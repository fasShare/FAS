#include "TcpServer.h"
#include <iostream>
#include "Socket.h"
#include <unistd.h>
#include <boost/bind.hpp>
#include "Dispatcher.h"
#include "HttpResponse.h"

using namespace std;

TcpServer::TcpServer(NetAddress addr) {
  handle = new Handle;

  serfd = socket(AF_INET, SOCK_STREAM, 0);

  if (-1 == serfd.bind(addr)) {
    cout << "bind error!" << endl;
    return;
  }

  if (-1 == serfd.listen(50)) {
    cout << "bind error!" << endl;
    return;
  }

  Events serevn;
  serevn.set_events(EPOLLIN);
  serevn.set_fd(serfd.get_sd());

  handle->set_event(serevn);
}

TcpServer::TcpServer() {
  handle = new Handle;

  serfd = socket(AF_INET, SOCK_STREAM, 0);
  NetAddress addr(8899, "127.0.0.1");

  if (-1 == serfd.bind(addr)) {
      cout << "bind error!" << endl;
      return;
  }

  if (-1 == serfd.listen(50)) {
    cout << "bind error!" << endl;
    return;
  }

  Events serevn;
  serevn.set_events(EPOLLIN);
  serevn.set_fd(serfd.get_sd());

  handle->set_event(serevn);
}


bool TcpServer::init(Dispatcher *dispatcher) {
  this->dispatch = dispatcher;
  handle->set_handle_event(boost::bind(&TcpServer::handle_event, this, _1));
  dispatch->add_handle(handle);

  return true;
}


Dispatcher* TcpServer::get_dispatcher() {
  return dispatch;
}


void TcpServer::handle_event(Events* event) {
    cout << "new http in sd = " << event->get_fd() << endl;

    int sd = accept(event->get_fd(), NULL, NULL);

    if (sd == -1) {
      return ;
    }
}
