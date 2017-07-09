#include <stdio.h>
#include <MultiProcessTcpServer.h>

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

int main () {
	MultiProcessTcpServer* server = MultiProcessTcpServer::Instance();
	server->setConnMessageCallback(ConnMessageCallback);
	server->start();
}
