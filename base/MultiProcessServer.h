#ifndef FAS_MULTIPROCESSSERVER_H
#define FAS_MULTIPROCESSSERVER_H
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <new>
#include <signal.h>

#include <Log.h>
#include <PipeFd.h>
#include <EventLoop.h>
#include <MultiProcessServer.h>
#include <ProcessServer.h>
#include <Environment.h>
#include <FasInfo.h>


#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_array.hpp>

namespace fas {
template <typename SERVER>
class MultiProcessServer : boost::noncopyable {
public:
    typedef typename SERVER::TcpConnMessageCallback TcpConnMessageCallback;
    typedef typename SERVER::OnConnectionCallBack OnConnectionCallBack;
    typedef typename SERVER::OnConnectionRemovedCallBack OnConnectionRemovedCallBack;
    ~MultiProcessServer();
	static MultiProcessServer *Instance();	

    static void signalHandler(int signo);
    
    bool reloadInfo();

	void setConnMessageCallback(TcpConnMessageCallback callback);
	void setConnBuildCallback(OnConnectionCallBack callback);
	void setConnRemoveCallback(OnConnectionRemovedCallBack callback);
    bool start();
private:
    class destroy_multiprocess {
        public:
            ~destroy_multiprocess() {
                if (MultiProcessServer<SERVER>::instance) {
                    delete MultiProcessServer<SERVER>::instance;
                }
            }
    };

    MultiProcessServer();
    std::vector<boost::shared_ptr<PipeFd>> pipes_;
    std::vector<boost::shared_ptr<ProcessServer<SERVER>>> process_;
    std::vector<pid_t> pids_;
    int threadNum_;
    EventLoop *loop_;  //do not used by serevr_
    boost::shared_ptr<SERVER> server_;
    bool quit_;
    bool waiting_;
    sigset_t maskset_;
    sigset_t maskold_;
    sigset_t waitset_;

	TcpConnMessageCallback messageCb_;
	OnConnectionCallBack connBuildCb_;
	OnConnectionRemovedCallBack connRemoveCb_;

	static MultiProcessServer<SERVER> *instance;
    static destroy_multiprocess ds_mulp; //要在其他地方定义
};

template <typename SERVER>
fas::MultiProcessServer<SERVER> *fas::MultiProcessServer<SERVER>::instance = nullptr;
template <typename SERVER>
typename fas::MultiProcessServer<SERVER>::destroy_multiprocess fas::MultiProcessServer<SERVER>::ds_mulp;
template <typename SERVER>
fas::MultiProcessServer<SERVER>::MultiProcessServer():
    pipes_(),
    process_(),
    pids_(),
    threadNum_(0),
    loop_(nullptr),
    server_(nullptr),
    quit_(false),
    waiting_(false) {
        sigemptyset(&maskset_);
        sigemptyset(&maskold_);
        sigemptyset(&waitset_);
    }

template <typename SERVER>
fas::MultiProcessServer<SERVER> *fas::MultiProcessServer<SERVER>::Instance() {
    if (nullptr == instance) {
        instance = new (std::nothrow) MultiProcessServer;
        //模板中静态变量在不使用时可能不会被定义
        (void)ds_mulp;
    }
    return instance;
}

template <typename SERVER>
void fas::MultiProcessServer<SERVER>::signalHandler(int signo) {
    if (SIGINT == signo) {
        std::string cmd = "[quit]";
        for (size_t i = 0; i < instance->pids_.size(); i++) {
            write(instance->pipes_[i]->getWriteEnd(), cmd.c_str(), cmd.size());
        }
        LOGGER_TRACE("Recive a SIGQUIT, will be exit.");
        instance->quit_ = true;
    } else if (SIGQUIT == signo) {
        std::string cmd = "[quit]";
        for (size_t i = 0; i < instance->pids_.size(); i++) {
            write(instance->pipes_[i]->getWriteEnd(), cmd.c_str(), cmd.size());
        }
        LOGGER_TRACE("Recive a SIGQUIT, will be exit.");
    } else if (SIGUSR1 == signo) {
        //do some things.
    } else if (SIGUSR2 == signo) {
        //do some things.
    } else if (SIGCHLD == signo) {
        LOGGER_TRACE("Maybe a multiPrecess server child exit.");
        int status = 0;
        if (-1 == waitpid(-1, &status, WNOHANG | WCONTINUED)) {
            LOGGER_ERROR("wait child error.");
            return;
        }

        if (WIFEXITED(status)) {
            LOGGER_TRACE("Child terminated normally.");
        }
        if (WIFSIGNALED(status)) {
            LOGGER_TRACE("The child process was terminated by a signal:" << WTERMSIG(status));
            if (WCOREDUMP(status)) {
                LOGGER_TRACE("The child terminated produced a core dump");
            }
        }
    }
}

template <typename SERVER>
bool fas::MultiProcessServer<SERVER>::reloadInfo() {
    if (!ENV_INIT()) {
        std::cout << "Environment init error!" << std::endl;
        return false;
    } else {
        LOGGER_TRACE("Environment init success!");
        return true;
    }
}

template <typename SERVER>
void fas::MultiProcessServer<SERVER>::setConnMessageCallback(TcpConnMessageCallback callback) {
    messageCb_ = callback;
}
template <typename SERVER>
void fas::MultiProcessServer<SERVER>::setConnBuildCallback(OnConnectionCallBack callback) {
    connBuildCb_ = callback;
}
template <typename SERVER>
void fas::MultiProcessServer<SERVER>::setConnRemoveCallback(OnConnectionRemovedCallBack callback) {
    connRemoveCb_ = callback;
}

#define ProcessNum 2

template <typename SERVER>
bool fas::MultiProcessServer<SERVER>::start() {
    InitGoogleLog("./faslog");
    //block some signal
    sigaddset(&maskset_, SIGPIPE);
    sigaddset(&maskset_, SIGALRM);
    sigaddset(&maskset_, SIGCONT);
    sigaddset(&maskset_, SIGSTOP);
    if (-1 == sigprocmask(SIG_BLOCK, &maskset_, &maskold_)) {
        std::cout << "mask signal set (SIGPIPE, SIGALRM, SIGCONT, SIGSTOP) error. in MultiProcessServer" << std::endl;
        return false;
    }
    //munipulate some signal
    signal(SIGCHLD, &MultiProcessServer<SERVER>::signalHandler);
    signal(SIGQUIT, &MultiProcessServer<SERVER>::signalHandler);
    signal(SIGUSR1, &MultiProcessServer<SERVER>::signalHandler);
    signal(SIGUSR2, &MultiProcessServer<SERVER>::signalHandler);
    signal(SIGINT, &MultiProcessServer<SERVER>::signalHandler);
    signal(SIGSTOP, &MultiProcessServer<SERVER>::signalHandler);

    while (!quit_) {
        if (!reloadInfo()) {
            return false;            
        }
        LOGGER_TRACE("Reload info succeed, Multi Server begin to start.");

        short port = GET_FAS_INFO()->getServerPort();
        if (port < 0) {
            LOGGER_ERROR("get server　Port error!");
            port = 6686;
        }   
        std::string ip = GET_FAS_INFO()->getServerIp();
        if (ip.empty()) {
            LOGGER_ERROR("get server　ip error!");
            ip = "127.0.0.1";
        }   
        int thread_num = GET_FAS_INFO()->getThreadNum();
        if (thread_num < 0) {
            LOGGER_ERROR("get server　thread_num error!");
            thread_num = 4;
        }   
        //由于epollfd是全局唯一的，最好在子进程里面创建，不然多个子进程可能会共享一个。
        server_ = boost::shared_ptr<SERVER>(new SERVER(nullptr, NetAddress(AF_INET, port, ip.c_str()), thread_num));
        if (!server_) {
            LOGGER_ERROR("New Server error in MultiProcessServer.");
            return false;
        }

        for (int idx = 0; idx < ProcessNum; idx++) {
            auto pipe = boost::shared_ptr<PipeFd>(new PipeFd());        
            if (!pipe) {
                LOGGER_ERROR("New pipe fd array error.");
                return false;
            }
            if (-1 == ::pipe(pipe->End)) {
                LOGGER_ERROR("Pipe error in MultiProcessServer : " << ::strerror(errno));
                return false;
            }
            pipes_.push_back(pipe);
            LOGGER_TRACE("idx = " << idx << " read = " << pipes_[idx]->getReadEnd() << " write = " << pipes_[idx]->getWriteEnd());
        }
        for (int idx = 0; idx < ProcessNum; ++idx) {
            auto proce = boost::shared_ptr<ProcessServer<SERVER>>(new (std::nothrow)  ProcessServer<SERVER>(server_, pipes_[idx]));
            if (!proce) {
                LOGGER_ERROR("New ProcessServer in MultiProcessServer.");
                return false;
            }
            process_.push_back(proce);
        }

        for (size_t idx = 0; idx < process_.size(); ++idx) { 
            //再循环中创建进程，要保证在子进程使用idx的时候，idx的值未变。
            pid_t pid = fork();
            if (pid == 0) {
                if (this->messageCb_) {
                    process_[idx]->setConnMessageCallback(this->messageCb_);
                } else {
                    LOGGER_WARN("Server message callback wasn't set.");
                }
                if (this->connBuildCb_) {
                    process_[idx]->setConnBuildCallback(this->connBuildCb_);
                } else {
                    LOGGER_WARN("Server conn build callback wasn't set.");
                }
                process_[idx]->initProc("./faslog");
                process_[idx]->startLoop();
                LOGGER_TRACE("a child quit. idx = " << idx);
                exit(EXIT_SUCCESS);
            } else if (pid < 0)  {
                LOGGER_ERROR("Fork child error in MultiProcessServer.");
            }
            //master进程记录所有子进程的pid
            pids_.push_back(pid);
            //关闭管道的读端,master主要用来通知子进程,后期可能会添加上读，让父子进程交互。	
            pipes_[idx]->closeReadEnd();
        }

        waiting_ = true;
        while (waiting_ && (!quit_)) {
            sleep(1);
        }
        waiting_ =  false;
    }

    return true;
}

template <typename SERVER>
fas::MultiProcessServer<SERVER>::~MultiProcessServer() {
    LOGGER_TRACE("MultiProcessServer will be destroyed.");
    quit_ = true;
    if (loop_) {
        delete loop_;
    }
}

}

#endif
