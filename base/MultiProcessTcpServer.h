#ifndef FAS_MULTIPROCESSTCPSERVER_H
#define FAS_MULTIPROCESSTCPSERVER_H
#include <vector>

#include <PipeFd.h>
#include <EventLoop.h>
#include <TcpServer.h>
#include <ProcessTcpServer.h>

#include <boost/noncopyable.hpp>
#include <boost/shared_array.hpp>

namespace fas {

class MultiProcessTcpServer : boost::noncopyable {
public:
    ~MultiProcessTcpServer();
	static MultiProcessTcpServer *Instance();	

    static void signalHandler(int signo);
    
    bool reloadInfo();

	void setConnMessageCallback(TcpServer::TcpConnMessageCallback callback);
	void setConnBuildCallback(TcpServer::OnConnectionCallBack callback);
	void setConnRemoveCallback(TcpServer::OnConnectionRemovedCallBack callback);
    bool start();
private:
    class destroy_multiprocess {
        public:
            ~destroy_multiprocess() {
                if (MultiProcessTcpServer::instance) {
                    delete MultiProcessTcpServer::instance;
                }
            }
    };

    MultiProcessTcpServer();
    std::vector<boost::shared_ptr<PipeFd>> pipes_;
    std::vector<boost::shared_ptr<ProcessTcpServer>> process_;
    std::vector<pid_t> pids_;
    int threadNum_;
    EventLoop *loop_;  //do not used by serevr_
    boost::shared_ptr<TcpServer> server_;
    bool quit_;
    bool waiting_;
    sigset_t maskset_;
    sigset_t maskold_;
    sigset_t waitset_;

	TcpServer::TcpConnMessageCallback messageCb_;
	TcpServer::OnConnectionCallBack connBuildCb_;
	TcpServer::OnConnectionRemovedCallBack connRemoveCb_;

	static MultiProcessTcpServer *instance;
    static destroy_multiprocess ds_mulp; //要在其他地方定义
};

}

#endif
