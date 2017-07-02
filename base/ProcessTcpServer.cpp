#include <string.h>

#include <ProcessTcpServer.h>
#include <TcpServer.h>
#include <Log.h>

#include <boost/bind.hpp>

fas::ProcessTcpServer::ProcessTcpServer(TcpServer* server, PipeFd *pipe, EventLoop *loop) :
    server_(server),
    pipe_(pipe),
    loop_(loop),
	readBuffer_(1024) {
}

fas::ProcessTcpServer::~ProcessTcpServer() {
    //server_->stop();
    loop_->quit();
    pipe_->closeWriteEnd();
    pipe_->closeReadEnd();
}

void fas::ProcessTcpServer::resetLoop(EventLoop *loop) {
	loop_ = loop;
}

bool fas::ProcessTcpServer::start() {
	if (!loop_) {
		LOGGER_ERROR("ProcessTcpServer's loop_ is nullptr.");
		return false;
	}
	//本进程的server在启动之时，必须使用本进程的EventLoop.
	server_->resetLoop(loop_);
	server_->start();
	if (!initReadEndHandle()) {
		LOGGER_ERROR("Init readEnd handle error. readEnd = " << pipe_->getReadEnd());	
		return false;
	}
	LOGGER_TRACE("Before add handle sd = " << pipe_->getReadEnd());	
	loop_->addHandle(readEndHandle_);
	LOGGER_TRACE("After add handle sd = " << pipe_->getReadEnd());	
	loop_->loop();
    
	return true;
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
