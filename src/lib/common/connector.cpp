//
// Created by victor on 19-3-8.
//

#include "connector.h"

namespace vzc {

Connector::Connector(EventLoop::Ptr event_loop)
    : event_loop_(event_loop), fd_(INVALID_SOCKET), connect_event_(nullptr) {

}

Connector::~Connector() {
  if(connect_event_) {
    if(event_pending(connect_event_, EV_WRITE, nullptr)) {
      event_del(connect_event_);
    }
    event_free(connect_event_);
    connect_event_ = nullptr;
  }
}

bool Connector::Start(const InetAddr& addr) {
  addr_ = addr;

  fd_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (INVALID_SOCKET == fd_) {
    LOG_ERROR("Create socket failed!");
    return false;
  }

  if (-1 == evutil_make_socket_nonblocking(fd_)) {
    LOG_ERROR("Make socket nonblocking failed!");
    evutil_closesocket(fd_);
    fd_ = INVALID_SOCKET;
    return false;
  }

  if (-1 == ::connect(fd_, (sockaddr *) addr_.GetAddr(), (socklen_t) sizeof(sockaddr_in))) {
    int e = evutil_socket_geterror(fd);
    if (e == EINTR || e == EINPROGRESS) {
      connect_event_ = event_new(event_loop_->base(), fd_, EV_WRITE, ConnectCallback, this);
      if (!connect_event_) {
        LOG_ERROR("Create connect event failed!");
        evutil_closesocket(fd_);
        fd_ = INVALID_SOCKET;
        return false;
      }
      if (-1 == event_add(connect_event_, nullptr)) {
        LOG_ERROR("Add connect event failed!");
        evutil_closesocket(fd_);
        fd_ = INVALID_SOCKET;
        return false;
      }
      return true;
    } else {
      LOG_ERROR("Connect failed!");
      evutil_closesocket(fd_);
      fd_ = INVALID_SOCKET;
      return false;
    }
  } else {
    SignalConnected(shared_from_this(), fd_, addr_);
  }

  return true;
}

void Connector::ConnectCallback(evutil_socket_t fd, int16_t events, void *ctx) {
  Connector::Ptr live_this = reinterpret_cast<Connector*>(ctx)->shared_from_this();
  live_this->OnConnect(fd, events);
}

void Connector::OnConnect(evutil_socket_t fd, int16_t events) {
  int e;
  ev_socklen_t elen = sizeof(e);

  if (-1 == ::getsockopt(fd, SOL_SOCKET, SO_ERROR, (void *) &e, &elen)) {
    LOG_ERROR_FMT("Connect failed! error: %d", EVUTIL_SOCKET_ERROR());
    SignalError(shared_from_this());
    return;
  }

  if (e) {
    if (e == EINTR || e == EINPROGRESS) {
      if (-1 == event_add(connect_event_, nullptr)) {
        LOG_ERROR_FMT("Add connect event failed! error: %d", EVUTIL_SOCKET_ERROR());
        SignalError(shared_from_this());
        return;
      }
      return;
    }
    LOG_ERROR_FMT("Connect failed! error: %d!", e);
    SignalError(shared_from_this());
    return;
  }

  SignalConnected(shared_from_this(), fd, addr_);
}

}