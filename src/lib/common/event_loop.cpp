//
// Created by victor on 19-3-6.
//

#include "event_loop.h"

namespace vzc {

EventLoop::EventLoop() : event_base_(nullptr) {

}

EventLoop::~EventLoop() {
  UnInitialize();
}

bool EventLoop::Initialize() {
  if (event_base_) {
    LOG_ERROR("This event loop already initialized!");
    return false;
  }

  event_base_ = event_base_new();
  if (!event_base_) {
    LOG_ERROR("Create event base failed!");
    return false;
  }
  return true;
}

void EventLoop::Run() {
  event_base_loop(event_base_, EVLOOP_NO_EXIT_ON_EMPTY);
}

void EventLoop::Stop() {
  event_base_loopbreak(event_base_);
}

void EventLoop::UnInitialize() {
  if (event_base_) {
    event_base_free(event_base_);
    event_base_ = nullptr;
  }
}
}
