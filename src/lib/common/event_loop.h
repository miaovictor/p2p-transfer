//
// Created by victor on 19-3-6.
//

#ifndef SRC_LIB_COMMON_EVENT_LOOP_H
#define SRC_LIB_COMMON_EVENT_LOOP_H

#include "common/global.h"
#include "common/noncopyable.hpp"
#include "event2/event.h"

namespace vzc {
class EventLoop : public noncopyable,
                  public std::enable_shared_from_this<EventLoop> {
 public:
  typedef std::shared_ptr<EventLoop> Ptr;
  EventLoop();
  virtual ~EventLoop();

 public:
  bool Initialize();
  void Run();
  void Stop();
  void UnInitialize();

  struct event_base *base() const {
    return event_base_;
  }

 private:
  struct event_base *event_base_;
};
}

#endif //SRC_LIB_COMMON_EVENT_LOOP_H
