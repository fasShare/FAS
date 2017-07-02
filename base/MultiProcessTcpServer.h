#ifndef FAS_MULTIPROCESSTCPSERVER_H
#define FAS_MULTIPROCESSTCPSERVER_H
#include <boost/noncopyable.hpp>
#include <vector>

#include <PipeFd.h>
#include <EventLoop.h>
#include <TcpServer.h>
#include <ProcessTcpServer.h>

namespace fas {

class MultiProcessTcpServer : boost::noncopyable {
public:
	static MultiProcessTcpServer *Instance();	

    static void signalHandler(int signo);
    
    bool reloadInfo();

	void setConnMessageCallback(TcpServer::TcpConnMessageCallback callback);
	void setConnBuildCallback(TcpServer::OnConnectionCallBack callback);
	void setConnRemoveCallback(TcpServer::OnConnectionRemovedCallBack callback);
    bool start();
private:
    MultiProcessTcpServer();
    ~MultiProcessTcpServer();
    PipeFd *pipes_;
    std::vector<ProcessTcpServer *> process_;
    std::vector<pid_t> pids_;
    int threadNum_;
    EventLoop *loop_;
    TcpServer *server_;
    bool quit_;
    bool waiting_;
    sigset_t maskset_;
    sigset_t maskold_;
    sigset_t waitset_;

	TcpServer::TcpConnMessageCallback messageCb_;
	TcpServer::OnConnectionCallBack connBuildCb_;
	TcpServer::OnConnectionRemovedCallBack connRemoveCb_;

	static MultiProcessTcpServer *instance;
};

}

#endif
