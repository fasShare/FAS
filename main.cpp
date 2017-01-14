#include <iostream>
#include "Dispatcher.h"
#include "TcpServer.h"
#include "Timestamp.h"
#include <Log.h>

using namespace std;

int main()
{
//    Dispatcher *dis = new Dispatcher;
//    TcpServer *ser = new TcpServer;

//    ser->init(dis);

//    dis->loop();


    Log log;

    log << "今天，玩的还算高兴吧! 心情能打多少分呢!" << Log::log_endl;

    log.fflush();



    return 0;
}

