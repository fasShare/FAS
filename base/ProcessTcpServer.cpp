#include <ProcessTcpServer.h>
#include <TcpServer.h>
#include <Log.h>

fas::ProcessTcpServer::ProcessTcpServer(TcpServer* server, PipeFd *pipe, EventLoop *loop) :
    server_(server),
    pipe_(pipe),
    loop_(loop) {
}

fas::ProcessTcpServer::~ProcessTcpServer() {
    //server_->stop();
    loop_->quit();
    pipe_->closeWriteEnd();
    pipe_->closeReadEnd();
}

bool fas::ProcessTcpServer::start() {
    return true;
}

void fas::ProcessTcpServer::eventHandle(const fas::Events& event, Timestamp time) {    
    if (event.containsAtLeastOneEvents(POLLIN | POLLPRI | POLLRDHUP)) {
        //read
    }   

    if (event.containsAtLeastOneEvents(POLLERR | POLLNVAL)) {
        //error
    }   

    if ((event.containsAtLeastOneEvents(POLLHUP)) && !(event.containsAtLeastOneEvents(POLLIN))) {
        //close
    }   

    if (event.containsAtLeastOneEvents(POLLOUT)) {
        //write
    }   

    if (event.containsAtLeastOneEvents(POLLNVAL)) {
        LOGGER_DEBUG("events contains POLLNVAL!");
    }   

}
