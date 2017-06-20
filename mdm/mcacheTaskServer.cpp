#include "mcacheTaskServer.h"
#include <boost/bind.hpp>
#include <Log.h>

fas::mdm::mcacheTaskServer::mcacheTaskServer(hashSPtr hash, fas::EventLoop* loop,
                                   const fas::NetAddress& addr,
                                   int threadNum) :
	loop_(loop),
	tcpSer_(loop_, addr, threadNum),
	reqHandles_(),
	hash_(hash) {
	tcpSer_.setOnConnectionCallBack(boost::bind(&mcacheTaskServer::OnNewConnection, this, _1));
	tcpSer_.setOnConnRemovedCallBack(boost::bind(&mcacheTaskServer::OnConnectionRemoved, this, _1));
}

void fas::mdm::mcacheTaskServer::OnNewConnection(TcpConnShreadPtr conn) {
	LOGGER_TRACE("mcacheTaskServer::OnNewConnection");
	// req must not be shared_ptr, or conn will be referenced by itself.
	// you can analyse it by youself seriously.
	std::shared_ptr<mcacheTaskHandle> reqHandle = std::make_shared<mcacheTaskHandle>(hash_);
	conn->setOnMessageCallBack(boost::bind(&mcacheTaskHandle::OnMessageCallback, reqHandle, _1, _2, _3));
	this->reqHandles_[connkey_t(conn->getConnfd(), conn.get())] = reqHandle;
}

void fas::mdm::mcacheTaskServer::OnConnectionRemoved(connkey_t key) {
	LOGGER_TRACE("mcacheTaskServer::OnConnectionRemoved");
	loop_->assertInOwnerThread();
	if (this->reqHandles_.find(key) == this->reqHandles_.end()) {
		return;
	}

	int ret = this->reqHandles_.erase(key);
	assert(ret == 1);
}

bool fas::mdm::mcacheTaskServer::start() {
	return tcpSer_.start();
}

fas::mdm::mcacheTaskServer::~mcacheTaskServer() {

}
