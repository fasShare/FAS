#include <string.h>

#include <ProcessTcpServer.h>
#include <Environment.h>
#include <TcpServer.h>
#include <Log.h>

#include <boost/bind.hpp>

fas::ProcessTcpServer::ProcessTcpServer(boost::shared_ptr<TcpServer> server, boost::shared_ptr<PipeFd> pipe) :
    server_(server),
    pipe_(pipe),
    loop_(nullptr),
	readBuffer_(1024),
    readEndHandle_(nullptr),
    reloaderTimer_(nullptr) {
}

fas::ProcessTcpServer::~ProcessTcpServer() {
    //不要在析构函数中做太多工作？
    //因为这个ProcessTcpServer是在父进程中创建的，所以在父进程中也会调用这个析构。
    //如果在这里面直接释放父进程中未构造完成的对象，就会出现段错误,要做有效行判断。
    if (loop_) {
        delete loop_;
        loop_ = nullptr;
    }
    LOGGER_TRACE("ProcessTcpServer wil be destroyed in process " << getpid());
}

bool fas::ProcessTcpServer::initProc(const std::string& logpf) {
    CloseGoogleLog();
    //每个进程进行日志的重新初始化，将日志打印到含有自己pid的日志文件中。
    InitGoogleLog(logpf);
    loop_ = new (std::nothrow) EventLoop;
    if (!loop_) {
        LOGGER_ERROR("New EventLoop error in MultiProcessTcpServer child process.");
        return false;
    }
    if (pipe_) {
        pipe_->closeWriteEnd();
    }
	//本进程的server在启动之时，必须使用本进程的EventLoop.
    if (server_) {
        if (nullptr != server_->getLoop()) {
            LOGGER_WARN("TcpServer's loop should be set in ProcessTcpServer, EventLoop is reset in ProcessTcpServer. usage:TcpServer(nullptr, address, threadNum)");
        }
        server_->resetLoop(loop_);
    } else {
        LOGGER_ERROR("ProcessTcpServer's TcpServer is nullptr.");
        return false;
    }
    if (messageCb_) {
        server_->setMessageCallback(messageCb_);
    }
    if (this->connBuildCb_) {
        server_->setOnConnectionCallBack(this->connBuildCb_);
    }
    if (this->connRemoveCb_) {
        server_->setOnConnRemovedCallBack(connRemoveCb_);
    }

	if ((pipe_) && !initReadEndHandle()) {
		LOGGER_ERROR("Init readEnd handle error. readEnd = " << pipe_->getReadEnd());	
		return false;
	}

    if (!reloaderTimer_) {
        reloaderTimer_ = new Timer(boost::bind(&ProcessTcpServer::reloaderTimerfunc, this), addTime(Timestamp::now(), 20), 20);
    }

    return true;
}

bool fas::ProcessTcpServer::startLoop() {
	if (!loop_) {
		LOGGER_ERROR("ProcessTcpServer's loop_ is nullptr.");
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

void fas::ProcessTcpServer::reloaderTimerfunc() {
    if (GET_ENV()->check_load()) {
        LOGGER_ERROR("Reloader success in process " << getpid());
    } else {
        LOGGER_ERROR("Reloader failed in process " << getpid());
    }
}

bool fas::ProcessTcpServer::initReadEndHandle() {
	readEndHandle_ = new (std::nothrow) Handle(loop_, fas::Events(pipe_->getReadEnd(), kReadEvent));
	if (!readEndHandle_) {
		return false;
	}
	readEndHandle_->setHandleRead(boost::bind(&ProcessTcpServer::handleRead, this, _1, _2));
	readEndHandle_->setHandleWrite(boost::bind(&ProcessTcpServer::handleWrite, this, _1, _2));
	readEndHandle_->setHandleError(boost::bind(&ProcessTcpServer::handleError, this, _1, _2));
	readEndHandle_->setHandleClose(boost::bind(&ProcessTcpServer::handleClose, this, _1, _2));
	return true;
}
//在各个事件处理函数中添加判断，处理事件。
void fas::ProcessTcpServer::handleRead(const fas::Events& event, Timestamp time) {
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
void fas::ProcessTcpServer::handleWrite(const fas::Events& event, Timestamp time) {

} 
void fas::ProcessTcpServer::handleError(const fas::Events& event, Timestamp time) {
	LOGGER_ERROR("Handle error.");
} 
void fas::ProcessTcpServer::handleClose(const fas::Events& event, Timestamp time) {
	loop_->quit();
} 
void fas::ProcessTcpServer::setConnMessageCallback(fas::TcpServer::TcpConnMessageCallback callback) {
    messageCb_ = callback;
}
void fas::ProcessTcpServer::setConnBuildCallback(fas::TcpServer::OnConnectionCallBack callback) {
    connBuildCb_ = callback;
}
void fas::ProcessTcpServer::setConnRemoveCallback(fas::TcpServer::OnConnectionRemovedCallBack callback) {
    connRemoveCb_ = callback;
}

