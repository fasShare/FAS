#include <sys/eventfd.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>


int CreateEventfd() {
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0) {
    std::cout << "Failed in eventfd" << std::endl;
    ::abort();
  }
  return evtfd;
}

void WakeUp(int wakeUpFd) {
  uint64_t one = 1;
  ssize_t n = ::write(wakeUpFd, &one, sizeof one);
  if (n != sizeof one) {
    // FIXME : use Log
    std::cout << "wakeup() writes " << n << " bytes instead of 8" << std::endl;
  } else {
     std::cout << "wakeup() writes " << one  << std::endl;
  }

}

void HandWakeUp(int wakeUpFd) {
  uint64_t one = 1;
  ssize_t n = ::read(wakeUpFd, &one, sizeof one);
  if (n != sizeof one){
    // FIXME : use Log
    std::cout << "HandWakeUp() reads " << n << " bytes instead of 8" << std::endl;
  } else {
    std::cout << "HandWakeUp() reads " << one << std::endl;
  }
}

/*!
 * 本测试程序主要测试eventFd的使用，当我们从一个线程把handle出到另外一个线程的EventLoop时，
 * 另外一个线程可能正在polling其他handle的事件，如果我们不进行线程的唤醒，那么只有等到有一个handle事件发生，
 * 或则polling超时我们才能把新的handle添加到polling队列中，这样可能会延迟handle回调函数的执行，
 * eventFd主要用来唤醒目标线程，然后直接把新handle添加到polling队列中。
 *
 *
 * eventfd在TcpServer产生新的TcpConnection是被使用，
 * 因为要通过robin算法把新的TCPConnection添加到下一个工作线程中。
 */

int main() {
  int wakeUpFd = CreateEventfd();
#if 0
  // If no WakeUp(), HandWakeUp() will reads -1 bytes instead of 8
  HandWakeUp(wakeUpFd);
#endif
#if 1
  WakeUp(wakeUpFd);
  HandWakeUp(wakeUpFd);
#endif
}
