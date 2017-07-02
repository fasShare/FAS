#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <new>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <Log.h>
#include <MultiProcessTcpServer.h>
#include <ProcessTcpServer.h>
#include <Environment.h>
#include <FasInfo.h>

#include <boost/bind.hpp>

fas::MultiProcessTcpServer *fas::MultiProcessTcpServer::instance = nullptr;
fas::MultiProcessTcpServer::MultiProcessTcpServer():
    pipes_(nullptr),
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

fas::MultiProcessTcpServer *fas::MultiProcessTcpServer::Instance() {
	if (nullptr == instance) {
		instance = new (std::nothrow) MultiProcessTcpServer;
	}
	return instance;
}

void fas::MultiProcessTcpServer::signalHandler(int signo) {
   if (SIGINT == signo) {
		std::string cmd = "[quit]";
		for (int i = 0; i < instance->pids_.size(); i++) {
			write(instance->pipes_[i].getWriteEnd(), cmd.c_str(), cmd.size());
		}
		LOGGER_TRACE("Recive a SIGQUIT, will be exit.");
        instance->quit_ = true;
   } else if (SIGQUIT == signo) {
		std::string cmd = "[quit]";
		for (int i = 0; i < instance->pids_.size(); i++) {
			write(instance->pipes_[i].getWriteEnd(), cmd.c_str(), cmd.size());
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

bool fas::MultiProcessTcpServer::reloadInfo() {
    if (!ENV_INIT()) {
        std::cout << "Environment init error!" << std::endl;
        return false;
    } else {
        LOGGER_TRACE("Environment init success!");
        return true;
    }
}

void fas::MultiProcessTcpServer::setConnMessageCallback(fas::TcpServer::TcpConnMessageCallback callback) {
	messageCb_ = callback;
}
void fas::MultiProcessTcpServer::setConnBuildCallback(fas::TcpServer::OnConnectionCallBack callback) {
	connBuildCb_ = callback;
}
void fas::MultiProcessTcpServer::setConnRemoveCallback(fas::TcpServer::OnConnectionRemovedCallBack callback) {
	connRemoveCb_ = callback;
}

#define ProcessNum 2

bool fas::MultiProcessTcpServer::start() {
    InitGoogleLog("./faslog");
    //block some signal
    sigaddset(&maskset_, SIGPIPE);
    sigaddset(&maskset_, SIGALRM);
    sigaddset(&maskset_, SIGCONT);
    sigaddset(&maskset_, SIGSTOP);
    if (-1 == sigprocmask(SIG_BLOCK, &maskset_, &maskold_)) {
        std::cout << "mask signal set (SIGPIPE, SIGALRM, SIGCONT, SIGSTOP) error. in MultiProcessTcpServer" << std::endl;
        return false;
    }
    //munipulate some signal
	signal(SIGCHLD, &MultiProcessTcpServer::signalHandler);
	signal(SIGQUIT, &MultiProcessTcpServer::signalHandler);
	signal(SIGUSR1, &MultiProcessTcpServer::signalHandler);
	signal(SIGUSR2, &MultiProcessTcpServer::signalHandler);
	signal(SIGINT, &MultiProcessTcpServer::signalHandler);
	signal(SIGSTOP, &MultiProcessTcpServer::signalHandler);

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
        server_ = new (std::nothrow) TcpServer(nullptr, NetAddress(AF_INET, port, ip.c_str()), thread_num);
        if (!server_) {
            LOGGER_ERROR("New TcpServer error in MultiProcessTcpServer.");
            return false;
        }

		pipes_ = new (std::nothrow) PipeFd[ProcessNum];        
        if (!pipes_) {
            LOGGER_ERROR("New pipe fd array error.");
            return false;
        }
        for (int idx = 0; idx < ProcessNum; idx++) {
            if (-1 == ::pipe(pipes_[idx].End)) {
                LOGGER_ERROR("Pipe error in MultiProcessTcpServer : " << ::strerror(errno));
                delete server_;
                delete[] pipes_;
                return false;
            }
			LOGGER_TRACE("idx = " << idx << " read = " << pipes_[idx].getReadEnd() << " write = " << pipes_[idx].getWriteEnd());
        }
        for (int idx = 0; idx < ProcessNum; ++idx) {
            ProcessTcpServer *proce = new (std::nothrow)  ProcessTcpServer(server_, pipes_ + idx, nullptr);
            if (!proce) {
                LOGGER_ERROR("New ProcessTcpServer in MultiProcessTcpServer.");
                delete server_;
                delete[] pipes_;
                return false;
            }
            process_.push_back(proce);
        }
    
        for (size_t idx = 0; idx < process_.size(); ++idx) { 
			//再循环中创建进程，要保证在子进程使用idx的时候，idx的值未变。
            pid_t pid = fork();
            if (pid == 0) {
				CloseGoogleLog();
				//每个进程进行日志的重新初始化，将日志打印到含有自己pid的日志文件中。
				InitGoogleLog("./faslog");
        		EventLoop *loop = new (std::nothrow) EventLoop;
        		if (!loop) {
            		LOGGER_ERROR("New EventLoop error in MultiProcessTcpServer child process.");
            		return false;
        		}
                pipes_[idx].closeWriteEnd();
				process_[idx]->resetLoop(loop);
				server_->setMessageCallback(messageCb_);
                process_[idx]->start();
			    LOGGER_TRACE("a child quit. idx = " << idx);
				exit(EXIT_SUCCESS);
            } else if (pid < 0)  {
                LOGGER_ERROR("Fork child error in MultiProcessTcpServer.");
            }
			//master进程记录所有子进程的pid
            pids_.push_back(pid);
			//关闭管道的读端,master主要用来通知子进程,后期可能会添加上读，让父子进程交互。	
			pipes_[idx].closeReadEnd();
        }

        waiting_ = true;
        while (waiting_ && (!quit_)) {
			sleep(1);
        }
        waiting_ =  false;
        // fix me : close all child process.
    }

    delete[] pipes_;
    pipes_ = nullptr;
    //some clear
    return true;
}

fas::MultiProcessTcpServer::~MultiProcessTcpServer() {
    if (!pipes_) {
        delete[] pipes_;
        pipes_ = nullptr;
    }
    quit_ = true;
}
