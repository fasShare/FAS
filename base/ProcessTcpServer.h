#ifndef FAS_PROCESSTCPSERVER_H
#define FAS_PROCESSTCPSERVER_H
#include <boost/noncopyable.hpp>

#include <PipeFd.h>
#include <TcpServer.h>
#include <Handle.h>
#include <EventLoop.h>
#include <Buffer.h>

namespace fas {

class ProcessTcpServer : boost::noncopyable {
public:
    ProcessTcpServer(TcpServer* server, PipeFd *pipe, EventLoop *loop);
    ~ProcessTcpServer();
    bool start();
	void resetLoop(EventLoop *loop);
	bool initReadEndHandle();
    void handleRead(const fas::Events& event, Timestamp time);
    void handleWrite(const fas::Events& event, Timestamp time);
    void handleError(const fas::Events& event, Timestamp time);
    void handleClose(const fas::Events& event, Timestamp time);
private:
    TcpServer *server_;
    PipeFd *pipe_;
    EventLoop *loop_;
    int threadNum_;
    Handle *readEndHandle_;
	Buffer readBuffer_;
};

}

#endif
