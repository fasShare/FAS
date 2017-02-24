#include <iostream>
#include <EventLoop.h>
#include <EventLoopThreadPool.h>
#include <TcpServer.h>
#include <Timestamp.h>
#include <Log.h>
#include <pthread.h>
#include <sys/syscall.h>

using namespace std;

int main()
{
    EventLoop *loop = new EventLoop;
    TcpServer *ser = new TcpServer(loop, NetAddress(8899, "127.0.0.1"));

    ser->start();
    loop->loop();
}

