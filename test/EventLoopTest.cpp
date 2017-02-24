#include <iostream>
#include <EventLoop.h>
#include <Log.h>
#include <Thread.h>
#include <boost/bind.hpp>

using namespace std;

void threadfunc(EventLoop *loop) {
    //loop->loop();
}

int main() {
    EventLoop *loop = new EventLoop();
    Thread thread(boost::bind(threadfunc, loop));

    loop->assertInOwner();

    thread.start();

    thread.join();

    return 0;
}
