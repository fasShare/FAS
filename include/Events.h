#ifndef FAS_EVENTS_H
#define FAS_EVENTS_H
#include <iostream>


#include <Types.h>


#include <boost/static_assert.hpp>

BOOST_STATIC_ASSERT_MSG(EPOLLIN == POLLIN, "EPOLLIN != POLLIN");
BOOST_STATIC_ASSERT_MSG(EPOLLOUT == POLLOUT, "EPOLLOUT != POLLOUT");
BOOST_STATIC_ASSERT_MSG(EPOLLPRI == POLLPRI, "EPOLLPRI != POLLPRI");
BOOST_STATIC_ASSERT_MSG(EPOLLERR == POLLERR, "EPOLLERR != POLLERR");
BOOST_STATIC_ASSERT_MSG(EPOLLRDHUP == POLLRDHUP, "EPOLLRDHUP != POLLRDHUP");

const events_t kNoneEvent = 0;
const events_t kReadEvent = POLLIN | POLLPRI;
const events_t kWriteEvent = POLLOUT;

/*!
 * \brief The Events class
 * The Events class be used to manager the IO event.such as POLLIN, POLLOUT, POLLPRI and so on.
 */
class Events {
public:
  Events(const int fd, uint32_t events);
  Events(const Events& events);
  /*!
   * \brief Events
   * \param events
   * only used after epoll_wait() return.
   */
  Events(const EpollEvent& events);
  /*!
   * \brief Events
   * \param events
   * only used after poll() return.
   */
  Events(const PollEvent& events);
   ~Events();

  /*!
   * \brief getFd
   * \return int
   * Get descriptor managed by this object.
   */
  int getFd() const;
  /*!
   * \brief getEvents
   * \return uint32_t
   * Get descriptor managed by this object. It can be ored by POLLIN, POLLOUT, POLLPRI ...
   */
  uint32_t getEvents() const;
  void updateEvents(uint32_t events);
  /*!
   * \brief addEvent
   * \param event
   * events_ |= event
   */
  void addEvent(uint32_t event);
  /*!
   * \brief deleteEvent
   * \param event
   * events_ &= ~event;
   */
  void deleteEvent(uint32_t event);

  /*!
   * \brief containsEvents
   * \param events
   * \return bool
   * if events_ contain all the event in events return true, otherwise return false
   */
  bool containsEvents(uint32_t events) const;
  /*!
   * \brief containsAtLeastOneEvents
   * \param events
   * \return bool
   * if events_ contain one or more event in events return true, otherwise return false
   */
  bool containsAtLeastOneEvents(uint32_t events) const;
  /*!
   * \brief epollEvents
   * \return struct epoll_event
   * Translate this Events object into struct epoll_event object which can be used to epoll.
   */
  EpollEvent epollEvents();

  /*!
   * \brief pollEvents
   * \return struct epollfd
   * Translate this Events object into struct epollfd object which can be used to poll.
   */
  PollEvent pollEvents();

private:
  int fd_;           /*!< descriptor */
  uint32_t events_;  /*!< ored by POLLIN, POLLOUT, POLLPRI ... */
};

std::ostream& operator<<(std::ostream& os, Events& events);

#endif // FAS_EVENTS_H
