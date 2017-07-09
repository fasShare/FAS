#ifndef FAS_PROCESSTCPSERVER_H
#define FAS_PROCESSTCPSERVER_H
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <PipeFd.h>
#include <TcpServer.h>
#include <Handle.h>
#include <EventLoop.h>
#include <Buffer.h>
#include <Timer.h>
#include <NetAddress.h>

namespace fas {

class ProcessTcpServer : boost::noncopyable {
public:
    ProcessTcpServer(boost::shared_ptr<TcpServer> server, boost::shared_ptr<PipeFd> pipe);
    ~ProcessTcpServer();
	
    bool startLoop();
    bool initProc(const std::string& info);

	bool initReadEndHandle();
    void handleRead(const fas::Events& event, Timestamp time);
    void handleWrite(const fas::Events& event, Timestamp time);
    void handleError(const fas::Events& event, Timestamp time);
    void handleClose(const fas::Events& event, Timestamp time);

    void reloaderTimerfunc();

    void setConnMessageCallback(TcpServer::TcpConnMessageCallback callback);
	void setConnBuildCallback(TcpServer::OnConnectionCallBack callback);
	void setConnRemoveCallback(TcpServer::OnConnectionRemovedCallBack callback);
private:
    boost::shared_ptr<TcpServer> server_;
    boost::shared_ptr<PipeFd> pipe_;
    EventLoop* loop_;
    int threadNum_;
	Buffer readBuffer_;
    Handle *readEndHandle_;
    Timer *reloaderTimer_;
	
    TcpServer::TcpConnMessageCallback messageCb_;
	TcpServer::OnConnectionCallBack connBuildCb_;
	TcpServer::OnConnectionRemovedCallBack connRemoveCb_;
};

}

#endif
