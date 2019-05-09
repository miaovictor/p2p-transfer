//
// Created by victor on 19-3-7.
//

#ifndef SRC_LIB_COMMON_ASYNC_PACKET_SOCKET_H
#define SRC_LIB_COMMON_ASYNC_PACKET_SOCKET_H

#include "json/json.h"
#include "common/event_loop.h"
#include "common/inet_addr.h"
#include "common/sigslot.h"
#include "common/data_buffer_pool.h"

namespace vzc {

class AsyncPacketSocket : public noncopyable,
                          public sigslot::has_slots<>,
                          public std::enable_shared_from_this<AsyncPacketSocket> {
 public:
  typedef std::shared_ptr<AsyncPacketSocket> Ptr;
  AsyncPacketSocket(EventLoop::Ptr event_loop, evutil_socket_t fd, const InetAddr& addr);
  virtual ~AsyncPacketSocket();

 public:
  sigslot::signal2<AsyncPacketSocket::Ptr, StpError> SignalError;
  sigslot::signal4<AsyncPacketSocket::Ptr, uint16_t , const char *, size_t> SignalRead;

 public:
  bool Initialize();
  const InetAddr& GetAddr() const;
  bool Send(uint16_t flag, const char* data, size_t size);
  bool SendJson(uint16_t flag, const Json::Value& data);
  void UnInitialize();

 private:
  static void ReadCallback(evutil_socket_t fd, int16_t events, void *ctx);
  static void WriteCallback(evutil_socket_t fd, int16_t events, void *ctx);
  static void ClosedCallback(evutil_socket_t fd, int16_t events, void *ctx);

 private:
  void OnRead(evutil_socket_t fd, int16_t events);
  void OnWrite(evutil_socket_t fd, int16_t events);
  void OnClosed(evutil_socket_t fd, int16_t events);

  bool AnalysisPacket();
  void TrySend();
  void EmitError(StpError error);

 private:
  EventLoop::Ptr event_loop_;
  evutil_socket_t fd_;
  InetAddr addr_;
  struct event *read_event_;
  struct event *write_event_;
  struct event *closed_event_;
  size_t drop_size_;

  char *recv_buffer_;
  size_t recv_size_;

  std::list<DataBuffer::Ptr> send_buffers_;

 private:
  static DataBufferPool::Ptr buffer_pool_;
};

}

#endif //SRC_LIB_COMMON_ASYNC_PACKET_SOCKET_H
