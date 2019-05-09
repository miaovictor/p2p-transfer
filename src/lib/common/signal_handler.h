//
// Created by victor on 19-3-7.
//

#ifndef SRC_LIB_COMMON_SIGNAL_HANDLER_H
#define SRC_LIB_COMMON_SIGNAL_HANDLER_H

#include "common/event_loop.h"
#include "common/sigslot.h"
#include <map>

namespace vzc {

class SignalHandler : public noncopyable,
                      public std::enable_shared_from_this<SignalHandler> {
 public:
  typedef std::shared_ptr<SignalHandler> Ptr;
  explicit SignalHandler(EventLoop::Ptr event_loop);
  virtual ~SignalHandler();

 public:
  sigslot::signal2<SignalHandler::Ptr, int> SignalEvent;

 public:
  bool ListenSignal(int signal);

 private:
  static void SignalCallback(evutil_socket_t fd, int16_t events, void* ctx);
  void OnSignal(evutil_socket_t fd, int16_t events);

 private:
  EventLoop::Ptr event_loop_;
  std::map<int, struct event *> signals_;
};

}

#endif //SRC_LIB_COMMON_SIGNAL_HANDLER_H
