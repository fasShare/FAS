#ifndef FAS_PROCESSSERVER_H
#define FAS_PROCESSSERVER_H
#include <string.h>

#include <Environment.h>
#include <Log.h>
#include <PipeFd.h>
#include <Handle.h>
#include <EventLoop.h>
#include <Buffer.h>
#include <Timer.h>
#include <NetAddress.h>

#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
namespace fas {

template <typename SERVER>
class ProcessServer : boost::noncopyable {
public:
    typedef typename SERVER::TcpConnMessageCallback TcpConnMessageCallback;
    typedef typename SERVER::OnConnectionCallBack OnConnectionCallBack;
    typedef typename SERVER::OnConnectionRemovedCallBack OnConnectionRemovedCallBack;
    ProcessServer(boost::shared_ptr<SERVER> server, boost::shared_ptr<PipeFd> pipe);
    ~ProcessServer();
	
    bool startLoop();
    bool initProc(const std::string& info);

	bool initReadEndHandle();
    void handleRead(const fas::Events& event, Timestamp time);
    void handleWrite(const fas::Events& event, Timestamp time);
    void handleError(const fas::Events& event, Timestamp time);
    void handleClose(const fas::Events& event, Timestamp time);

    void reloaderTimerfunc();

    void setConnMessageCallback(TcpConnMessageCallback callback);
	void setConnBuildCallback(OnConnectionCallBack callback);
	void setConnRemoveCallback(OnConnectionRemovedCallBack callback);
private:
    boost::shared_ptr<SERVER> server_;
    boost::shared_ptr<PipeFd> pipe_;
    EventLoop* loop_;
    int threadNum_;
	Buffer readBuffer_;
    Handle *readEndHandle_;
    Timer *reloaderTimer_;
	
    TcpConnMessageCallback messageCb_;
	OnConnectionCallBack connBuildCb_;
	OnConnectionRemovedCallBack connRemoveCb_;
};

template <typename SERVER>
fas::ProcessServer<SERVER>::ProcessServer(boost::shared_ptr<SERVER> server, boost::shared_ptr<PipeFd> pipe) :
    server_(server),
    pipe_(pipe),
    loop_(nullptr),
	readBuffer_(1024),
    readEndHandle_(nullptr),
    reloaderTimer_(nullptr) {
}

template <typename SERVER>
fas::ProcessServer<SERVER>::~ProcessServer() {
    //不要在析构函数中做太多工作？
    //因为这个ProcessServer是在父进程中创建的，所以在父进程中也会调用这个析构。
    //如果在这里面直接释放父进程中未构造完成的对象，就会出现段错误,要做有效行判断。
    if (loop_) {
        delete loop_;
        loop_ = nullptr;
    }
    LOGGER_TRACE("ProcessServer wil be destroyed in process " << getpid());
}

template <typename SERVER>
bool fas::ProcessServer<SERVER>::initProc(const std::string& logpf) {
    CloseGoogleLog();
    //每个进程进行日志的重新初始化，将日志打印到含有自己pid的日志文件中。
    InitGoogleLog(logpf);
    loop_ = new (std::nothrow) EventLoop;
    if (!loop_) {
        LOGGER_ERROR("New EventLoop error in MultiProcessServer child process.");
        return false;
    }
    if (pipe_) {
        pipe_->closeWriteEnd();
    }
	//本进程的server在启动之时，必须使用本进程的EventLoop.
    if (server_) {
        if (nullptr != server_->getLoop()) {
            LOGGER_WARN("TcpServer's loop should be set in ProcessServer, EventLoop is reset in ProcessServer. usage:TcpServer(nullptr, address, threadNum)");
        }
        server_->setLoop(loop_);
    } else {
        LOGGER_ERROR("ProcessServer's TcpServer is nullptr.");
        return false;
    }
    if (messageCb_) {
        server_->setMessageCallback(messageCb_);
    }
    if (this->connBuildCb_) {
        server_->setOnConnectionCallBack(this->connBuildCb_);
    }

	if ((pipe_) && !initReadEndHandle()) {
		LOGGER_ERROR("Init readEnd handle error. readEnd = " << pipe_->getReadEnd());	
		return false;
	}

    if (!reloaderTimer_) {
        reloaderTimer_ = new Timer(boost::bind(&ProcessServer<SERVER>::reloaderTimerfunc, this), addTime(Timestamp::now(), 20), 20);
    }

    return true;
}

template <typename SERVER>
bool fas::ProcessServer<SERVER>::startLoop() {
	if (!loop_) {
		LOGGER_ERROR("ProcessServer's loop_ is nullptr.");
		return false;
	}
	server_->start();
	loop_->addHandle(readEndHandle_);
    loop_->addTimer(this->reloaderTimer_);
	loop_->loop();
    
    //不要在析构函数中做这步。
    CloseGoogleLog();
	return true;
}

template <typename SERVER>
void fas::ProcessServer<SERVER>::reloaderTimerfunc() {
    if (GET_ENV()->check_load()) {
        LOGGER_ERROR("Reloader success in process " << getpid());
    } else {
        LOGGER_ERROR("Reloader failed in process " << getpid());
    }
}

template <typename SERVER>
bool fas::ProcessServer<SERVER>::initReadEndHandle() {
	readEndHandle_ = new (std::nothrow) Handle(loop_, fas::Events(pipe_->getReadEnd(), kReadEvent));
	if (!readEndHandle_) {
		return false;
	}
	readEndHandle_->setHandleRead(boost::bind(&ProcessServer<SERVER>::handleRead, this, _1, _2));
	readEndHandle_->setHandleWrite(boost::bind(&ProcessServer<SERVER>::handleWrite, this, _1, _2));
	readEndHandle_->setHandleError(boost::bind(&ProcessServer<SERVER>::handleError, this, _1, _2));
	readEndHandle_->setHandleClose(boost::bind(&ProcessServer<SERVER>::handleClose, this, _1, _2));
	return true;
}
//在各个事件处理函数中添加判断，处理事件。
template <typename SERVER>
void fas::ProcessServer<SERVER>::handleRead(const fas::Events& event, Timestamp time) {
	if (event.getFd() == pipe_->getReadEnd()) {		
		int error = 0;
		if (0 > readBuffer_.readFd(event.getFd(), &error)) {
			LOGGER_ERROR("Read pipe read end error : " << ::strerror(error));
			return;
		}
		const char *cmddelimitend = readBuffer_.findChars("]", 1);
		if (NULL ==  cmddelimitend) {
			// receive cmd is not complete.
			return;
		}
		std::string cmd = readBuffer_.retrieveAsString(cmddelimitend + 1 - readBuffer_.peek());
		if (0 == strcmp("[quit]", cmd.c_str())) {
			LOGGER_TRACE("Receive quit cmd, process server will be exit.");
			loop_->quit();
		}
	}
} 
template <typename SERVER>
void fas::ProcessServer<SERVER>::handleWrite(const fas::Events& event, Timestamp time) {

} 
template <typename SERVER>
void fas::ProcessServer<SERVER>::handleError(const fas::Events& event, Timestamp time) {
	LOGGER_ERROR("Handle error.");
} 
template <typename SERVER>
void fas::ProcessServer<SERVER>::handleClose(const fas::Events& event, Timestamp time) {
	loop_->quit();
} 
template <typename SERVER>
void fas::ProcessServer<SERVER>::setConnMessageCallback(TcpConnMessageCallback callback) {
    messageCb_ = callback;
}
template <typename SERVER>
void fas::ProcessServer<SERVER>::setConnBuildCallback(OnConnectionCallBack callback) {
    connBuildCb_ = callback;
}
template <typename SERVER>
void fas::ProcessServer<SERVER>::setConnRemoveCallback(OnConnectionRemovedCallBack callback) {
    connRemoveCb_ = callback;
}

}

#endif
