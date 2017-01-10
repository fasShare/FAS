#ifndef EVENTSPOLLER_H
#define EVENTSPOLLER_H
#include <vector>
#include "Events.h"

class Events;
using namespace std;

class Poller
{
private:
  Poller(const Poller &);
  Poller& operator=(const Poller &);
public:
  Poller();
  virtual ~Poller();
  virtual bool Poller_event_add(Events* events) = 0;
  virtual bool Poller_event_mod(Events* events) = 0;
  virtual bool Poller_event_del(Events* events) = 0;
  virtual int Poller_loop(vector<Events> &events, int max_events, int timeout) = 0;
  static Poller* Poller_create();
};

#endif // EVENTSPOLLER_H
