#include <iostream>
#include <Dispatcher.h>
#include <DispatcherThreadPool.h>
#include <TcpServer.h>
#include <Timestamp.h>
#include <Log.h>
#include <pthread.h>
#include <sys/syscall.h>

using namespace std;

int main()
{
    Dispatcher *dis = new Dispatcher;
    TcpServer *ser = new TcpServer;
    ser->init(dis);

    DispatcherThreadPool pool(dis, 4);
    pool.start();

    dis->dispatch();
}

