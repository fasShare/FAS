#include "TcpServer.h"
#include <iostream>
#include "Socket.h"

using namespace std;

TcpServer::TcpServer(NetAddress addr) {
  Socket serfd = socket(AF_INET, SOCK_STREAM, 0);

  if (-1 == serfd.Socket_bind(addr)) {
    cout << "bind error!" << endl;
    return;
  }

  if (-1 == serfd.Socket_listen(50)) {
    cout << "bind error!" << endl;
    return;
  }

  Events serevn;
  serevn.Events_set_events(EPOLLIN | EPOLLET);
  serevn.Events_set_fd(serfd.Socket_get_sd());

  Executor_set_event(serevn);
}

TcpServer::TcpServer() {
  Socket serfd = socket(AF_INET, SOCK_STREAM, 0);
  NetAddress addr(8899, "127.0.0.1");

  if (-1 == serfd.Socket_bind(addr)) {
      cout << "bind error!" << endl;
      return;
  }

  if (-1 == serfd.Socket_listen(50)) {
    cout << "bind error!" << endl;
    return;
  }

  Events serevn;
  serevn.Events_set_events(EPOLLIN | EPOLLET);
  serevn.Events_set_fd(serfd.Socket_get_sd());

  Executor_set_event(serevn);
}

void TcpServer::Executor_handle_event(Events* event) {
  cout << "Tcpserver fd:" << event->Events_get_fd() << endl;
}

