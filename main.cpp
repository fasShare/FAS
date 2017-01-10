#include <iostream>
#include "Dispatcher.h"
#include "TcpServer.h"

using namespace std;

int main()
{
    Dispatcher *dis = new Dispatcher;
    TcpServer *ser = new TcpServer;

    ser->init(dis);

    dis->loop();
    return 0;
}

