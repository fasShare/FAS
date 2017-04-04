#include <HttpServer.h>
#include <EventLoop.h>
#include <NetAddress.h>

using namespace fas;
using namespace fas::http;

int main() {
    EventLoop *loop = new EventLoop;

    HttpServer *ser = new HttpServer(loop, NetAddress());

    ser->start();

    loop->loop();
}
