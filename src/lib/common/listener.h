//
// Created by victor on 19-3-7.
//

#ifndef SRC_LIB_COMMON_LISTENER_H
#define SRC_LIB_COMMON_LISTENER_H

#include "common/event_loop.h"
#include "common/inet_addr.h"
#include "common/sigslot.h"
#include "event2/listener.h"

namespace vzc {

class Listener : public noncopyable,
                 public std::enable_shared_from_this<Listener> {
 public:
  typedef std::shared_ptr<Listener> Ptr;
  explicit Listener(EventLoop::Ptr event_loop);
  virtual ~Listener();

 public:
  sigslot::signal3<Listener::Ptr, evutil_socket_t, const InetAddr&> SignalAccept;
  sigslot::signal1<Listener::Ptr> SignalError;

 public:
  bool Start(const InetAddr& addr, int backlog = 10);

  const InetAddr& GetAddr() const;

  int GetPort() const;

 private:
  static void AcceptCallback(struct evconnlistener *listener,
                             evutil_socket_t fd,
                             struct sockaddr *saddr,
                             int slen,
                             void *ctx);
  static void ErrorCallback(struct evconnlistener *listener, void *ctx);

 private:
  void OnAccept(struct evconnlistener *listener,
                evutil_socket_t fd,
                struct sockaddr *saddr,
                int slen);
  void OnError(struct evconnlistener *listener);

 private:
  EventLoop::Ptr event_loop_;
  struct evconnlistener *listener_;
  evutil_socket_t fd_;
  InetAddr addr_;
};

}

#endif //SRC_LIB_COMMON_LISTENER_H
