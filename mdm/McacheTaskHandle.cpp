#include <McacheTaskHandle.h>
#include <ConsistencyHash.h>
#include <HashNode.h>
#include <boost/core/ignore_unused.hpp>
#include <EventLoop.h>
#include <Log.h>

fas::mdm::McacheTaskHandle::McacheTaskHandle(HashSPtr hash) :
	mTask_(hash),
	hash_(hash) {

}

void fas::mdm::McacheTaskHandle::OnMessageCallback(TcpConnShreadPtr conn, boost::shared_ptr<fas::Buffer> buffer, fas::Timestamp time) {
	boost::ignore_unused(conn, time);
	std::string str(buffer->peek(), 0, buffer->readableBytes());
	LOGGER_TRACE(str << " bytes:" << buffer->readableBytes());
	if (buffer->findCRLF() != NULL) {
		LOGGER_TRACE("find crlf.");
	}
	if (!mTask_.taskCommunition(conn, buffer)) {
		if (mTask_.getState() == McacheTask::TaskState::BAD) {
			conn->shutdown();
		}
		return;
	}
}

void fas::mdm::McacheTaskHandle::sendDataToMemcached() {

}

fas::mdm::McacheTaskHandle::~McacheTaskHandle() {
}

