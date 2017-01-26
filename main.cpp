#include <iostream>
#include <Dispatcher.h>
#include <TcpServer.h>
#include <Timestamp.h>
#include <Log.h>
#include <pthread.h>
#include <sys/syscall.h>

using namespace std;



class list_st{
public:
  list_st(int tid){
		if (last == NULL) {
			last = this;
		}
		if (head == NULL) {
			pre = this;
			next = this;
			head = this;
		} else {
			head->pre = this;
			next = head;
			head = this;
			last->next = head;
		}
	  count++;
    id = tid;
  }
  ~list_st(){
     pre->next = next;
  }

  list_st *Next(){
      return next;
  }
	list_st* Pre() {
		return pre;
	}

	static list_st* Head(){
		return head;
	}

  int Count() {
      return count;
  }

	int Tid() {
		return id;
	}

private:
  static int count;
  int id;
	static list_st *last;
  static list_st *head;
  list_st *next;
	list_st *pre;
};

list_st *list_st::head = NULL;
list_st *list_st::last = NULL;
int list_st::count = 0;


int main()
{
    Dispatcher *dis = new Dispatcher;
    TcpServer *ser = new TcpServer;

    ser->init(dis);

    dis->loop();
}

