#include <iostream>
#include <Socket.h>
#include <NetAddress.h>
#include <Thread.h>

using namespace fas;
using namespace std;

void work() {
    while (1) {
        Socket client(AF_INET, SOCK_STREAM, 0);
        NetAddress addr(AF_INET, 6686, "192.168.1.3");
        if (!client.connect(addr)) {
            return;
        }
        char buf[512];
        int bytes = rand() % 511;
        for (int i = 0; i < bytes; i++) {
            buf[i] = 's';
        }
        buf[bytes] = 0;
        write(client.getSocket(), buf, bytes);
        sleep(rand()%3);
        write(client.getSocket(), buf, bytes);
        client.close();
    }
}

#define THREAD_NUM 20

int main() {
    std::vector<Thread*> threads;
    for (int i = 0; i < THREAD_NUM; i++) {
        Thread *thread = new Thread(work);
        threads.push_back(thread);
        threads.back()->start();
    }
    
    for (int i = 0; i < THREAD_NUM; i++) {
        threads[i]->join();
    }
    return 0;
}
