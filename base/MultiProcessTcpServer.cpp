#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <new>
#include <signal.h>

#include <Log.h>
#include <FasInfo.h>
#include <Environment.h>
#include <MultiProcessTcpServer.h>

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

void fas::MultiProcessTcpServer::signalHandler(int signo) {
   if (SIGINT == signo) {
        //do some things.
   } else if (SIGQUIT == signo) {
        waiting_ = true;
        quit_ = true;
   } else if (SIGUSR1 == signo) {
        //do some things.
   } else if (SIGUSR2 == signo) {
        //do some things.
   } else if (SIGCHLD == signo) {
        int status = 0;
        if (-1 == waitpid(-1, &status, WNOHANG)) {
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

#define ProcessNum 4

bool fas::MultiProcessTcpServer::start() {

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
    sigaddset(&waitset_, SIGCHLD);
    sigaddset(&waitset_, SIGINT);
    sigaddset(&waitset_, SIGQUIT);
    sigaddset(&waitset_, SIGUSR1);
    sigaddset(&waitset_, SIGUSR2);

    if (-1 == sigprocmask(SIG_BLOCK, &maskset_, &maskold_)) {
        std::cout << "block signal error in MultiProcessTcpServer" << std::endl;
        return false;
    }

    while (!quit_) {
        if (reloadInfo()) {
            return false;            
        }
        LOGGER_TRACE("Reload info succeed, Multi Server begin to start.");
        
        pipes_ = new (std::nothrow) PipeFd[ProcessNum];
        if (!pipes_) {
            LOGGER_ERROR("New pipe fd array error.");
            return false;
        }
        loop_ = new (std::nothrow) EventLoop;
        if (!loop_) {
            delete[] pipes_;
            LOGGER_ERROR("New EventLoop error in MultiProcessTcpServer.");
            return false;
        }
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
        server_ = new (std::nothrow) TcpServer(loop_, NetAddress(AF_INET, port, ip.c_str()), thread_num);
        if (!server_) {
            delete loop_;
            delete[] pipes_;
            LOGGER_ERROR("New TcpServer error in MultiProcessTcpServer.");
            return false;
        }

        for (int i = 0; i < ProcessNum; i++) {
            if (-1 == ::pipe(pipes_[i].End)) {
                LOGGER_ERROR("Pipe error in MultiProcessTcpServer.");
                delete server_;
                delete loop_;
                delete[] pipes_;
                return false;
            }
        }
        for (int idx = 0; idx < ProcessNum; ++idx) {
            ProcessTcpServer *proce = new (std::nothrow)  ProcessTcpServer(server_, pipes_ + idx, loop_);
            if (!proce) {
                LOGGER_ERROR("New ProcessTcpServer in MultiProcessTcpServer.");
                delete server_;
                delete loop_;
                delete[] pipes_;
                return false;
            }
            process_.push_back(proce);
        }
    
        for (size_t idx = 0; idx < process_.size(); ++idx) {
            pid_t pid = fork();
            if (pid == 0) {
                process_[idx]->start();
                LOGGER_TRACE("A child quit.");
                return true;
            } else if (pid < 0)  {
                LOGGER_ERROR("Fork child error in MultiProcessTcpServer.");
            }
            pids_.push_back(pid);
        }

        waiting_ = true;
        while (waiting_ && (!quit_)) {
            int signo = -1;
            if (EINVAL == sigwait(&waitset_, &signo)) {
                LOGGER_ERROR("Signal set contains an invalid signal number.");
                //fix me : close all child process.
                return false;
            }

           signalHandler(signo);
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
}}
