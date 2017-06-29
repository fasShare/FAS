#include <ProcessTcpServer.h>
#include <TcpServer.h>

fas::ProcessTcpServer::ProcessTcpServer(TcpServer* server, PipeFd *pipe, EventLoop *loop) :
    server_(server),
    pipe_(pipe),
    loop_(loop) {
}

fas::ProcessTcpServer::~ProcessTcpServer() {
    //server_->stop();
    loop->quit();
    pipe_->closeWriteEnd();
    pipe_->closeReadEnd();
}

bool fas::ProcessTcpServer::start() {

}
