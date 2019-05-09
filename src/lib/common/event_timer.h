//
// Created by victor on 19-3-6.
//

#ifndef SRC_LIB_COMMON_EVENT_TIMER_H
#define SRC_LIB_COMMON_EVENT_TIMER_H

#include "common/event_loop.h"
#include "common/sigslot.h"

namespace vzc {

class EventTimer : public noncopyable,
                   public std::enable_shared_from_this<EventTimer> {
 public:
  typedef std::shared_ptr<EventTimer> Ptr;
  EventTimer(EventLoop::Ptr event_loop);
  virtual ~EventTimer();

 public:
  sigslot::signal1<EventTimer::Ptr> SignalTimer;

 public:
  bool Initialize(bool persist = true);
  bool Start(int64_t milliseconds);
  void Stop();
  void UnInitialize();

 private:
  static void TimerCallback(evutil_socket_t fd, int16_t events, void *ctx);
  void OnTimer(evutil_socket_t fd, int16_t events);

 private:
  EventLoop::Ptr event_loop_;
  struct timeval tv_;
  struct event *event_;
};

}

#endif //SRC_LIB_COMMON_EVENT_TIMER_H
