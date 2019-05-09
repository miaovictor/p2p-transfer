//
// Created by victor on 19-3-8.
//

#ifndef SRC_LIB_COMMON_CONNECTOR_H
#define SRC_LIB_COMMON_CONNECTOR_H

#include "common/event_loop.h"
#include "common/sigslot.h"
#include "common/inet_addr.h"

namespace vzc {

class Connector : public noncopyable,
                  public std::enable_shared_from_this<Connector> {
 public:
  typedef std::shared_ptr<Connector> Ptr;
  explicit Connector(EventLoop::Ptr event_loop);
  virtual ~Connector();

 public:
  sigslot::signal3<Connector::Ptr, evutil_socket_t, const InetAddr&> SignalConnected;
  sigslot::signal1<Connector::Ptr> SignalError;

 public:
  bool Start(const InetAddr& addr);

 private:
  static void ConnectCallback(evutil_socket_t fd, int16_t events, void *ctx);

 private:
  void OnConnect(evutil_socket_t fd, int16_t events);

 private:
  EventLoop::Ptr event_loop_;
  evutil_socket_t fd_;
  InetAddr addr_;
  struct event *connect_event_;
};

}

#endif //SRC_LIB_COMMON_CONNECTOR_H
