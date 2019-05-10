//
// Created by victor on 19-3-7.
//

#include "listener.h"

namespace vzc {

Listener::Listener(EventLoop::Ptr event_loop)
    : event_loop_(event_loop), listener_(nullptr) {

}

Listener::~Listener() {
  if (listener_) {
    evconnlistener_free(listener_);
    listener_ = nullptr;
  }
}

bool Listener::Start(const InetAddr& addr, int backlog) {
  int flags = LEV_OPT_CLOSE_ON_FREE | LEV_OPT_CLOSE_ON_EXEC | LEV_OPT_REUSEABLE;
  listener_ = evconnlistener_new_bind(
      event_loop_->base(),
      AcceptCallback,
      this,
      flags,
      backlog,
      (sockaddr *) addr.GetAddr(),
      sizeof(sockaddr));
  if (!listener_) {
    LOG_ERROR("Call evconnlistener_new_bind error!");
    return false;
  }

  evconnlistener_set_error_cb(listener_, ErrorCallback);

  fd_ = evconnlistener_get_fd(listener_);

  if(backlog == -1) {
    int enable = 1;
    if (-1 == setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable))) {
      LOG_ERROR("Set SO_REUSEADDR failed!");
      return false;
    }

    if (-1 == setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable))) {
      LOG_ERROR("Set SO_REUSEPORT failed!");
      return false;
    }

    if (-1 == setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable))) {
      LOG_ERROR("Set SO_KEEPALIVE failed!");
      return false;
    }
  }


  struct sockaddr_in bindaddr;
  socklen_t len = sizeof(bindaddr);
  if(0 != ::getsockname(fd_, (sockaddr*)&bindaddr, &len)) {
    LOG_ERROR_FMT("Call getsockname failed! errno: %d!", EVUTIL_SOCKET_ERROR());
    return false;
  }

  addr_ = InetAddr(bindaddr);

  return true;
}

const InetAddr &Listener::GetAddr() const {
  return addr_;
}

int Listener::GetPort() const {
  return addr_.GetPort();
}

void Listener::AcceptCallback(struct evconnlistener *listener,
                              evutil_socket_t fd,
                              struct sockaddr *saddr,
                              int slen,
                              void *ctx) {
  Listener::Ptr live_this = reinterpret_cast<Listener *>(ctx)->shared_from_this();
  live_this->OnAccept(listener, fd, saddr, slen);
}

void Listener::ErrorCallback(struct evconnlistener *listener, void *ctx) {
  Listener::Ptr live_this = reinterpret_cast<Listener *>(ctx)->shared_from_this();
  live_this->OnError(listener);
}

void Listener::OnAccept(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *saddr, int slen) {
  InetAddr addr(*(reinterpret_cast<struct sockaddr_in*>(saddr)));
  SignalAccept(shared_from_this(), fd, addr);
}

void Listener::OnError(struct evconnlistener *listener) {
  SignalError(shared_from_this());
}


}