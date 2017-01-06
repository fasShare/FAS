#include "TcpServer.h"
#include <iostream>
#include "Socket.h"
#include <unistd.h>
#include "HttpResponse.h"

using namespace std;

TcpServer::TcpServer(NetAddress addr) {
  serfd = socket(AF_INET, SOCK_STREAM, 0);

  if (-1 == serfd.Socket_bind(addr)) {
    cout << "bind error!" << endl;
    return;
  }

  if (-1 == serfd.Socket_listen(50)) {
    cout << "bind error!" << endl;
    return;
  }

  Events serevn;
  serevn.Events_set_events(EPOLLIN);
  serevn.Events_set_fd(serfd.Socket_get_sd());

  Executor_set_event(serevn);
}

TcpServer::TcpServer() {
  serfd = socket(AF_INET, SOCK_STREAM, 0);
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
  serevn.Events_set_events(EPOLLIN);
  serevn.Events_set_fd(serfd.Socket_get_sd());

  Executor_set_event(serevn);
}

void TcpServer::Executor_handle_event(Events* event) { 
    cout << "new http in sd = " << event->Events_get_fd() << endl;
	int sd = accept(event->Events_get_fd(), NULL, NULL);

    if (sd == -1) {

      return ;
    }

#if 0
    char buf[200] = {0};
    int maxLen = 200-1;
    int ret = read(sd, buf, maxLen);
    if (ret == 0) {
        cout << "Connection closed rsd : " << sd  << endl;
        close(sd);

        //FIXME : remove from Dispatcher

        return ;
    }

    if (ret == -1) {
        //FIXME :
    }

    if (ret < maxLen) {
        cout << "recv from sd : " << sd << " : "  <<  buf << endl;
        write(sd, buf, ret);
    }
    cout << "new http out" << endl;
#endif

    cout << "new sd: " << sd << endl;


    HttpResponse *http = new HttpResponse(sd);


    Events serevn;
    serevn.Events_set_events(EPOLLIN);
    serevn.Events_set_fd(sd);

    http->Executor_set_event(serevn);


    this->Executor_get_dispatcher()->Dispatcher_add_events(http);

	cout << "new http out" << endl;
}
