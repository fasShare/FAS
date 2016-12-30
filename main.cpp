#include <iostream>
#include "Epoll.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <memory>
#include "Dispatcher.h"
#include "Executor.h"
#include "NetAddress.h"
#include "Socket.h"
#include "TcpServer.h"

using namespace std;

int main()
{
  Dispatcher dispatcher;
  TcpServer *server = new TcpServer;
  dispatcher.Dispatcher_add_events(server);
  dispatcher.Dispatcher_loop();
  return 0;
}

