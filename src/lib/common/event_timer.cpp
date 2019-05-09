//
// Created by victor on 19-3-6.
//

#include "event_timer.h"

namespace vzc {

EventTimer::EventTimer(EventLoop::Ptr event_loop)
    : event_loop_(event_loop), event_(nullptr) {
}

EventTimer::~EventTimer() {
  UnInitialize();
}

bool EventTimer::Initialize(bool persist) {
  if (event_) {
    LOG_ERROR("This event timer already initialized!");
    return false;
  }

  short events = EV_TIMEOUT;
  if (persist) {
    events |= EV_PERSIST;
  }
  event_ = event_new(event_loop_->base(), -1, events, TimerCallback, this);
  if (!event_) {
    LOG_ERROR("Create event timer failed!");
    return false;
  }

  return true;
}

bool EventTimer::Start(int64_t milliseconds) {
  if (event_ && evtimer_pending(event_, &tv_) == 0) {
    tv_.tv_sec = milliseconds / 1000;
    tv_.tv_usec = (milliseconds % 1000) * 1000;
    event_add(event_, &tv_);
    return true;
  }
  return false;
}

void EventTimer::Stop() {
  if (event_ && evtimer_pending(event_, &tv_)) {
    event_del(event_);
  }
}

void EventTimer::TimerCallback(evutil_socket_t fd, int16_t events, void *ctx) {
  EventTimer::Ptr live_this = reinterpret_cast<EventTimer *>(ctx)->shared_from_this();
  live_this->OnTimer(fd, events);
}

void EventTimer::OnTimer(evutil_socket_t fd, int16_t events) {
  SignalTimer(shared_from_this());
}

void EventTimer::UnInitialize() {
  if (event_) {
    if(evtimer_pending(event_, &tv_)) {
      event_del(event_);
    }
    event_free(event_);
    event_ = nullptr;
  }
}

}