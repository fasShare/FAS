#include <iostream>
#include "include/Epoll.h"
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
#include "include/Dispatcher.h"
#include "include/Executor.h"
#include "include/NetAddress.h"
#include "include/Socket.h"
#include "include/TcpServer.h"

using namespace std;

int main()
{
  Dispatcher dispatcher;
  TcpServer *server = new TcpServer;
  dispatcher.Dispatcher_add_events(server);
  dispatcher.Dispatcher_loop();
  return 0;
}

