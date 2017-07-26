#include <assert.h>

#include <TcpConnBucket.h>
#include <MutexLocker.h>
#include <EventLoop.h>
#include <Log.h>

#include <boost/bind.hpp>

bool fas::TcpConnBucket::addTcpConnection(long tid, connkey_t key, boost::shared_ptr<TcpConnection>& conn) {
    MutexLocker locker(mutex_);
    if (conns_.end() == conns_.find(tid)) {
        conns_[tid] = std::map<connkey_t, boost::shared_ptr<TcpConnection>>(); 
    }
    if (conns_[tid].end() != conns_[tid].find(key)) {
        return false;
    }
    conns_[tid][key] = conn;
    return true;
}

void fas::TcpConnBucket::removeTcpConnection(long tid, connkey_t key) {
    MutexLocker locker(mutex_);
	auto iter = conns_.find(tid);
    assert(conns_.end() != iter);
	auto connmap = conns_[tid].find(key);
	assert(conns_[tid].end() != connmap);
	auto conn = connmap->second;
	EventLoop *clearloop = conn->getLoop();
	clearloop->queueInLoop(boost::bind(&TcpConnection::clearInLoop, conn));
    size_t n = conns_[tid].erase(key);
    assert(n == 1);
    LOGGER_ERROR("A TcpConnection was removed from bucket.");
}

