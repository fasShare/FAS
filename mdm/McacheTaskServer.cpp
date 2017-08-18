#include <McacheTaskServer.h>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <Log.h>

fas::mdm::McacheTaskServer::McacheTaskServer(HashSPtr hash, fas::EventLoop* loop,
                                   const fas::NetAddress& addr,
                                   int threadNum) :
	loop_(loop),
	tcpSer_(loop_, addr, threadNum),
	hash_(hash) {
	tcpSer_.setOnConnectionCallBack(boost::bind(&McacheTaskServer::OnNewConnection, this, _1));
}

void fas::mdm::McacheTaskServer::OnNewConnection(TcpConnShreadPtr conn) {
	LOGGER_TRACE("McacheTaskServer::OnNewConnection");
	// req must not be shared_ptr, or conn will be referenced by itself.
	// you can analyse it by youself seriously.
	boost::shared_ptr<McacheTaskHandle> reqHandle = boost::make_shared<McacheTaskHandle>(hash_);
	conn->setOnMessageCallBack(boost::bind(&McacheTaskHandle::OnMessageCallback, reqHandle, _1, _2, _3));
}

bool fas::mdm::McacheTaskServer::start() {
	return tcpSer_.start();
}

fas::mdm::McacheTaskServer::~McacheTaskServer() {

}
