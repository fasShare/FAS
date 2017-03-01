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
