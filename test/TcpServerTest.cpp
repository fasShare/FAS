#include <iostream>
#include <pthread.h>
#include <sys/syscall.h>


#include <EventLoop.h>
#include <Timestamp.h>
#include <Timer.h>
#include <TimerHeap.h>
#include <TimersScheduler.h>
#include <Log.h>
#include <TcpServer.h>
#include <TcpConnection.h>


#include <boost/bind.hpp>

/*!
 * 这是一个简单的echo测试程序，如果想改变这个服务器的功能可以改变函数TcpConnMessageCallback里面的逻辑，
 * 这个函数原型void TcpConnMessageCallback(TcpConnection *conn,
 *                          Buffer *buffer,
 *                          Timestamp time);
 * 当然，你也可以重新定义函数名，如，void tcpcb(TcpConnection *conn, Buffer *buffer, Timestamp time).
 *
 * 服务端使用的端口默认是8899， IP地址：127.0.0.1
 * 测试时可以在linux终端使用命令：nc 127.0.0.1 8899
 */

using namespace std;
using namespace fas;

void ConnMessageCallback(fas::TcpServer::TcpConnShreadPtr conn, Buffer *buffer, Timestamp time) {
  /*!
   * \brief str
   * 把收到的内容存到一个字符串中，供后面打印出来。
   */
  std::string str(buffer->retrieveAllAsString());
  std::cout << time.toFormattedString() << ":" << str << std::endl;
  conn->sendString(str);
  conn->shutdown();
}

int main()
{
    std::cout << "before loop" << std::endl;
    //这一步是必须的，EventLoop是程序的核心。
    EventLoop *loop = new EventLoop();
    std::cout << "after loop" << std::endl;
    //把loop传给TcpServer，也就是说，TcpServer里面的定时器事件，套接字监听，消息读写等事件，
    //都会在下面的loop循环中被polling监听。
    TcpServer *ser = new TcpServer(loop, NetAddress(AF_INET, 6686, "192.168.1.3"), 1);
    //当有客户段发来消息时，调用的函数，本函数写你处理消息的逻辑，消息被服务器框架存储在buffer里面。
    //这个buffer，是可以根据内容自动扩充空间的。
    ser->setMessageCallback(boost::bind(ConnMessageCallback, _1, _2, _3));

    ser->start();
    //最终程序会在这个loop函数里面进行各种事件的监听。
    loop->loop();
    delete loop;
}
