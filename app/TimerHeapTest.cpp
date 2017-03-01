#include <iostream>


#include <EventLoop.h>
#include <Timer.h>
#include <TimerHeap.h>


#include <boost/bind.hpp>

using namespace std;

void timer_func(int num) {
  cout << num << " timer is expired!" << endl;
}

int main() {
    Timer *timer1 = new Timer(boost::bind(timer_func, 1), addTime(Timestamp::now(), 0.1), 1);
    Timer *timer2 = new Timer(boost::bind(timer_func, 2), addTime(Timestamp::now(), 0.2), 1);
    Timer *timer3 = new Timer(boost::bind(timer_func, 3), addTime(Timestamp::now(), 0.3), 0);
    Timer *timer4 = new Timer(boost::bind(timer_func, 4), addTime(Timestamp::now(), 0.4), 0);
    Timer *timer5 = new Timer(boost::bind(timer_func, 5), addTime(Timestamp::now(), 0.5), 0);
    Timer *timer6 = new Timer(boost::bind(timer_func, 6), addTime(Timestamp::now(), 0.6), 0);
    Timer *timer7 = new Timer(boost::bind(timer_func, 7), addTime(Timestamp::now(), 0.7), 0);
    Timer *timer8 = new Timer(boost::bind(timer_func, 8), addTime(Timestamp::now(), 0.8), 0);
    Timer *timer9 = new Timer(boost::bind(timer_func, 9), addTime(Timestamp::now(), 0.9), 0);

    TimerHeap heap;

    heap.addTimer(timer1);
    heap.addTimer(timer2);
    heap.addTimer(timer3);
    heap.addTimer(timer4);
    heap.addTimer(timer5);
    heap.addTimer(timer6);
    heap.addTimer(timer7);
    heap.addTimer(timer8);
    heap.addTimer(timer9);
  #if 1
    Timestamp expired = timer5->getExpiration();
    std::vector<std::pair<Timestamp, Timer*>> expiredList;

    heap.getExpiredTimers(expiredList, expired);

    assert(heap.getTimerNum() == 4);
    assert(expiredList.size() == 5);

    heap.restartIntervalTimer(expiredList);

    for (auto iter = expiredList.begin(); iter != expiredList.end(); iter++) {
      if (iter->second != nullptr)
        iter->second->run();
    }

    assert(heap.getTimerNum() == 6);
  #endif

  #if 0
  cout << "---------------------------------------------------------" << endl;

    cout << heap.getTimerNum() << endl; // 9
    assert(heap.getEarlistExpiration() == timer1->getExpiration());
    heap.delTimer(timer1);
    cout << heap.getTimerNum() << endl; // 8
    assert(heap.getEarlistExpiration() == timer2->getExpiration());
    heap.delTimer(timer2);
    cout << heap.getTimerNum() << endl; // 7
    assert(heap.getEarlistExpiration() == timer3->getExpiration());
    heap.delTimer(timer3);
    cout << heap.getTimerNum() << endl; // 6
    assert(heap.getEarlistExpiration() == timer4->getExpiration());
    heap.delTimer(timer4);
    cout << heap.getTimerNum() << endl; // 5
    assert(heap.getEarlistExpiration() == timer5->getExpiration());
    heap.delTimer(timer5);
    cout << heap.getTimerNum() << endl; // 4
    assert(heap.getEarlistExpiration() == timer6->getExpiration());
    heap.delTimer(timer6);
    cout << heap.getTimerNum() << endl; // 3
    assert(heap.getEarlistExpiration() == timer7->getExpiration());
    heap.delTimer(timer7);
    cout << heap.getTimerNum() << endl; // 2
    assert(heap.getEarlistExpiration() == timer8->getExpiration());
    heap.delTimer(timer8);
    cout << heap.getTimerNum() << endl; // 1
    assert(heap.getEarlistExpiration() == timer9->getExpiration());
    heap.delTimer(timer9);
    cout << heap.getTimerNum() << endl; // 0

  cout << "---------------------------------------------------------" << endl;

  #endif
}
