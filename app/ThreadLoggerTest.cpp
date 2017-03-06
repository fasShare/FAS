#include <iostream>
#include <unistd.h>


#include <Log.h>
#include <Thread.h>


using namespace std;

/*!
 * \brief logger
 * 在多线程环境下可以尝试使用全局的线程特定数据实现Logger优化日志输出。
 */
PthreadSpecificData<Log> logger;

void threadFunc() {
  cout << "In thread "<< gettid() << " logger address : " << &(logger.Data()) << endl;
  // 保证其他线程在打印上面那条语句时当前线程还在运行，那样得到的地址才是不一样的，
  // 不然线程特定数据又被重复利用了。
  sleep(10);
}

#define THREAD_NUM 4

int main() {
  Thread threads[THREAD_NUM];

  for (auto i = 0; i < THREAD_NUM; i++) {
    ::sleep(1);
    threads[i].setThreadFunc(threadFunc);
    threads[i].start();
  }

  for(auto i = 0; i < THREAD_NUM; i++) {
    threads[i].join();
  }

  return 0;
}
