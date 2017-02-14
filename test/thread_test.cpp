#include <unistd.h>
#include <iostream>
#include <Thread.h>

using namespace std;

void threadfunc() {
	cout << "thread" << endl;
	sleep(100);
	cout << "thread exit" << endl;
}

int main() {
	Thread thread(threadfunc);
    thread.start();

    sleep(1);

	cout << "tid : " << gettid() << endl;
	cout << "pid : " << getpid() << endl;


    assert(!thread.MainThread());

	thread.join();
	return 0;
}
