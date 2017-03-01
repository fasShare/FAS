#include <iostream>

#include <Timer.h>
#include <TimerHeap.h>
#include <EventLoop.h>
#include <TimersScheduler.h>


#include <boost/bind.hpp>

using namespace std;


Timer *timer1 = nullptr;
Timer *timer2 = nullptr;
EventLoop *loop = nullptr;

void timer_func(int num) {
  assert(loop);
  static int firstCount = 0;
  if (num == 1) {
    ++firstCount;
    if (firstCount == 3) {
      loop->delTimer(timer2);
    }

    if (firstCount == 4) {
      loop->addTimer(timer1);
    }
  }
  cout << num << " timer is expired!" << endl;
}

int main() {

  loop = new EventLoop();

  timer1 = new Timer(boost::bind(timer_func, 1), addTime(Timestamp::now(), 0.1), 1);
  timer2 = new Timer(boost::bind(timer_func, 2), addTime(Timestamp::now(), 0.2), 1);
  Timer *timer3 = new Timer(boost::bind(timer_func, 3), addTime(Timestamp::now(), 0.3), 0);
  Timer *timer4 = new Timer(boost::bind(timer_func, 4), addTime(Timestamp::now(), 0.4), 0);
  Timer *timer5 = new Timer(boost::bind(timer_func, 5), addTime(Timestamp::now(), 0.5), 0);
  Timer *timer6 = new Timer(boost::bind(timer_func, 6), addTime(Timestamp::now(), 0.6), 0);
  Timer *timer7 = new Timer(boost::bind(timer_func, 7), addTime(Timestamp::now(), 0.7), 0);
  Timer *timer8 = new Timer(boost::bind(timer_func, 8), addTime(Timestamp::now(), 0.8), 0);
  Timer *timer9 = new Timer(boost::bind(timer_func, 9), addTime(Timestamp::now(), 0.9), 0);

  loop->addTimer(timer1);
  loop->addTimer(timer2);
  loop->addTimer(timer3);
  loop->addTimer(timer4);
  loop->addTimer(timer5);
  loop->addTimer(timer6);
  loop->addTimer(timer7);
  loop->addTimer(timer8);
  loop->addTimer(timer9);

  loop->loop();
}

