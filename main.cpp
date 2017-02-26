#include <iostream>
#include <pthread.h>
#include <sys/syscall.h>


#include <EventLoop.h>
#include <EventLoopThreadPool.h>
#include <TcpServer.h>
#include <Timestamp.h>
#include <Log.h>


#include <boost/bind.hpp>


using namespace std;

int main()
{
    EventLoop *loop = new EventLoop;
    TcpServer *ser = new TcpServer(loop, NetAddress(8899, "127.0.0.1"));

    ser->setMessageCallback(boost::bind(TcpConnMessageCallback, _1, _2, _3));

    ser->start();
    loop->loop();
}

