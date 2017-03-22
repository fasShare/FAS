#include <iostream>

#include <Timer.h>
#include <TimerHeap.h>
#include <EventLoop.h>
#include <TimersScheduler.h>


#include <boost/bind.hpp>

using namespace std;
using namespace fas;

Timer *timer1 = nullptr;
Timer *timer2 = nullptr;
EventLoop *loop = nullptr;

void timer_func(int num) {
  assert(loop);
  static int firstCount = 0;
  if (num == 1) {
    ++firstCount;
    if (firstCount == 3) {
      // 定时的内存释放又TimerHeap完成。
      loop->delTimer(timer2);
    }

    if (firstCount == 4) {
      loop->addTimer(timer1);
    }
  }
  cout << num << " timer is expired!" << endl;
}

/*!
 * 在看这个程序之前可以先测试下TimerHeapTest
 *
 * 这个程序演示了怎样把我们的定时器添加到loop中。
 *
 * EventLoop 最终还是通过TimersScheduler进行定时器的调度的。
 */

int main() {

  loop = new EventLoop();

  /*!
   * 在这9个定时当中，只有timer1，timer2会被反复调度，因为Timer的第三个参数为1,
   * 也就是说在第一次调度完成之后，会在后续每个1秒分别调度Timer1， Timer2，
   * 不过在timer_func中Timer2被Loop删除，最终只会有Timer以1秒间隔执行。
   */

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

