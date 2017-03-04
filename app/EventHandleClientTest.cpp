#include <iostream>  //cout, endl;
#include <strings.h> //bzero


#include <EventLoop.h>
#include <Events.h>
#include <Handle.h>
#include <Socket.h>
#include <NetAddress.h>
#include <Timestamp.h>
#include <Log.h>

using namespace std;

#define DATASIZE 200
#define BUFSIZE  (DATASIZE + 1)

EventLoop *loop = nullptr;
// 在EventLoop中监听client的可读可写事件。
Handle *handle = nullptr;

void HandleRead(Events event, Timestamp time) {
  (void)event;
  (void)time;
  char buf[BUFSIZE];
  bzero(buf, BUFSIZE);
  int ret = read(event.getFd(), buf, DATASIZE);
  if (ret > 0) {
    buf[ret] = 0;
    cout <<"read:" << buf << endl;
    handle->enableWrite();
    loop->modHandle(handle);
  } else if (ret == 0) {
    handle->disableRead();
    handle->disableWrite();
    loop->delHandle(handle);
  } else {
    LOGGER_TRACE << "read error :" << ::strerror(errno) << Log::CLRF;
    return;
  }
}

void HandleWrite(Events event, Timestamp time) {
  (void)event;
  (void)time;

  std::string buf;
  cout << "input:";
  cin >> buf;

  if (buf.size() > DATASIZE) {
    LOGGER_TRACE << "input too much bytes." << Log::CLRF;
    return;
  }
  // 可能一次写不完 ……
  int ret = write(event.getFd(), buf.c_str(), buf.size());
  if (ret == 0) {
    handle->disableRead();
    handle->disableWrite();
    loop->delHandle(handle);
  } else if (ret < 0) {
    LOGGER_TRACE << "write error :" << ::strerror(errno) << Log::CLRF;
    return;
  }
  handle->disableWrite();
  loop->modHandle(handle);

}

int main() {
  Socket client(AF_INET, SOCK_STREAM, 0);
  NetAddress addr(AF_INET, 8899, "127.0.0.1");

  loop = new EventLoop();
  handle = new Handle(loop, Events(client.getSocket(), kReadEvent | kWriteEvent));

  handle->setHandleRead(HandleRead);
  handle->setHandleWrite(HandleWrite);

  loop->addHandle(handle);

  client.setExecClose();
  client.setNoBlocking();

  //connect 可能会有错误提示，这是由于对于非阻塞socket，如果不能即可链接成功就会提示错误。
  client.connect(addr);

  loop->loop();
}
