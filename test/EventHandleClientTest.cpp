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
using namespace fas;

#define DATASIZE 200
#define BUFSIZE  (DATASIZE + 1)

EventLoop *loop = nullptr;
/*!
 * \brief handle
 * Handle 对象必须以new的方式创建，因为EventLoop是通过Handle的智能指针来管理handle对象的。
 */
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

    /**
     * 只有在需要写socket的时候才把socket的写事件监听打开，
     * 因为写一般什么时候下都可以写，如果写事件监听我们的程序会一直陷入这个写事件回调函数
     * 然后阻塞在cin，TcpConnection在进行写socket时也是在需要时代开socket写事件
     */
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
  // 在EventLoop中监听client的可读可写事件。
  handle = new Handle(loop, Events(client.getSocket(), kReadEvent | kWriteEvent));

  //为client的socket设置读事件回调函数，和写事件回调函数。
  handle->setHandleRead(HandleRead);
  handle->setHandleWrite(HandleWrite);

  //把handle添加到EventLoop进行事件监听
  loop->addHandle(handle);

  /**
   * 设置socket非阻塞，在子进程中关闭这个套接字，这里我们没有子进程。
   */
  client.setExecClose();
  client.setNoBlocking();

  //connect 可能会有错误提示，这是由于对于非阻塞socket，如果不能即可链接成功就会提示错误。
  //client链接到服务器。
  client.connect(addr);

  loop->loop();
}
