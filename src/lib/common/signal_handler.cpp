//
// Created by victor on 19-3-7.
//

#include "signal_handler.h"

namespace vzc {

SignalHandler::SignalHandler(EventLoop::Ptr event_loop)
    : event_loop_(event_loop) {

}

SignalHandler::~SignalHandler() {

}

bool SignalHandler::ListenSignal(int signal) {
  struct event *signal_event = evsignal_new(event_loop_->base(), signal, SignalCallback, this);
  if(!signal_event) {
    LOG_ERROR("Create signal event failed!");
    return false;
  }

  return evsignal_add(signal_event, NULL) == 0;
}

void SignalHandler::SignalCallback(evutil_socket_t fd, int16_t events, void *ctx) {
  SignalHandler::Ptr live_this = reinterpret_cast<SignalHandler *>(ctx)->shared_from_this();
  live_this->OnSignal(fd, events);
}

void SignalHandler::OnSignal(evutil_socket_t fd, int16_t events) {
  SignalEvent(shared_from_this(), fd);
}

}