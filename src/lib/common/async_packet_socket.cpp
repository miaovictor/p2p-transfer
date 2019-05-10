//
// Created by victor on 19-3-7.
//

#include "async_packet_socket.h"

#define MAX_PACKET_SIZE (10 * 1024)

namespace vzc {

struct PacketHeader {
  char v;
  char z;
  uint16_t flag;
  uint32_t size;
};

DataBufferPool::Ptr AsyncPacketSocket::buffer_pool_(new DataBufferPool(MAX_PACKET_SIZE));

AsyncPacketSocket::AsyncPacketSocket(EventLoop::Ptr event_loop, evutil_socket_t fd, const InetAddr& addr)
    : event_loop_(event_loop),
      fd_(fd),
      addr_(addr),
      read_event_(nullptr),
      write_event_(nullptr),
      closed_event_(nullptr),
      drop_size_(0) {

}

AsyncPacketSocket::~AsyncPacketSocket() {
  LOG_WARN("Destory AsyncPacketSocket");
  UnInitialize();
}

bool AsyncPacketSocket::Initialize() {
  recv_buffer_ = new char[MAX_PACKET_SIZE];
  recv_size_ = 0;

  struct event_base *base = event_loop_->base();

  read_event_ = event_new(base, fd_, EV_READ | EV_PERSIST, ReadCallback, this);
  if (!read_event_) {
    LOG_ERROR("Create read event failed!");
    return false;
  }

  write_event_ = event_new(base, fd_, EV_WRITE, WriteCallback, this);
  if (!write_event_) {
    LOG_ERROR("Create write event failed!");
    return false;
  }

  closed_event_ = event_new(base, fd_, EV_CLOSED, ClosedCallback, this);
  if (!closed_event_) {
    LOG_ERROR("Create closed event failed!");
    return false;
  }

  if (-1 == event_add(read_event_, nullptr)) {
    LOG_ERROR("Add read event failed!");
    return false;
  }

  if (-1 == event_add(closed_event_, nullptr)) {
    LOG_ERROR("Add closed event failed!");
    return false;
  }

  return true;
}

const InetAddr &AsyncPacketSocket::GetAddr() const {
  return addr_;
}

bool AsyncPacketSocket::Send(uint16_t flag, const char *data, size_t size) {
  if(size + sizeof(PacketHeader) > MAX_PACKET_SIZE) {
    LOG_WARN("Send packet is too large!");
    return false;
  }

  PacketHeader header;
  header.v = 'V';
  header.z = 'Z';
  header.flag = htons(flag);
  header.size = htonl(size);

  DataBuffer::Ptr buffer = buffer_pool_->TakeDataBuffer();
  buffer->WriteBytes((const char*)&header, sizeof(PacketHeader));
  if(size > 0) {
    buffer->WriteBytes(data, size);
  }
  send_buffers_.push_back(buffer);

  TrySend();
  return true;
}

bool AsyncPacketSocket::SendJson(uint16_t flag, const Json::Value &data) {
  Json::StreamWriterBuilder builder;
  std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
  std::ostringstream oss;
  writer->write(data, &oss);

  std::string send_data = oss.str();
  return Send(flag, send_data.data(), send_data.size());
}

void AsyncPacketSocket::ReadCallback(evutil_socket_t fd, int16_t events, void *ctx) {
  AsyncPacketSocket::Ptr live_this = reinterpret_cast<AsyncPacketSocket *>(ctx)->shared_from_this();
  live_this->OnRead(fd, events);
}

void AsyncPacketSocket::WriteCallback(evutil_socket_t fd, int16_t events, void *ctx) {
  AsyncPacketSocket::Ptr live_this = reinterpret_cast<AsyncPacketSocket *>(ctx)->shared_from_this();
  live_this->OnWrite(fd, events);
}

void AsyncPacketSocket::ClosedCallback(evutil_socket_t fd, int16_t events, void *ctx) {
  AsyncPacketSocket::Ptr live_this = reinterpret_cast<AsyncPacketSocket *>(ctx)->shared_from_this();
  live_this->OnClosed(fd, events);
}

void AsyncPacketSocket::OnRead(evutil_socket_t fd, int16_t events) {
  ssize_t ret = ::recv(fd, recv_buffer_ + recv_size_, MAX_PACKET_SIZE - recv_size_, 0);
  if (-1 == ret) {
    int e = evutil_socket_geterror(fd);
    if (e == EINTR || e == EAGAIN) {
      // 接收数据阻塞，再次尝试
      return;
    } else {
      // 接收数据出错，强制关闭
      LOG_ERROR_FMT("Receive data failed! socket error: %d!", e);
      EmitError(StpError::Socket_Receive);
    }
  } else if (0 == ret) {
    // 对方主动关闭
    LOG_WARN("Socket closed by peer!");
    EmitError(StpError::Socket_Closed);
  } else {
    recv_size_ += ret;
    if (!AnalysisPacket()) {
      EmitError(StpError::Socket_Packet);
    }
  }
}

void AsyncPacketSocket::OnWrite(evutil_socket_t fd, int16_t events) {
  TrySend();
}

void AsyncPacketSocket::OnClosed(evutil_socket_t fd, int16_t events) {
  // 对方主动关闭
  LOG_WARN("Socket closed by peer!");
  EmitError(StpError::Socket_Closed);
}

void AsyncPacketSocket::TrySend() {
  if (send_buffers_.size() == 0) return;

  while (send_buffers_.size()) {
    DataBuffer::Ptr buffer = send_buffers_.front();

    ssize_t ret = ::send(fd_, buffer->data_, buffer->size_, 0);
    if (ret >= 0) {
      if (ret == buffer->size_) {
        // 此buffer发送完毕
        send_buffers_.pop_front();
      } else {
        // 此buffer未发送完毕
        memmove(buffer->data_, buffer->data_ + ret, buffer->size_ - ret);
        buffer->size_ = buffer->size_ - ret;
      }
    } else {
      int e = evutil_socket_geterror(fd_);
      if (e == EINTR || e == EAGAIN) {
        if (write_event_ && event_pending(write_event_, EV_WRITE, nullptr) == 0) {
          if (-1 == event_add(write_event_, nullptr)) {
            LOG_ERROR("Add write event failed!");
            EmitError(StpError::Socket_Send);
          }
          break;
        } else {
          LOG_WARN("Already add write event!");
        }
      } else {
        LOG_ERROR_FMT("Send data failed! socket error: %d!", e);
        EmitError(StpError::Socket_Send);
        break;
      }
    }
  }
}

bool AsyncPacketSocket::AnalysisPacket() {
  if (recv_size_ == 0) return true;
  if (drop_size_ > 0) {
    if (drop_size_ >= recv_size_) {
      drop_size_ -= recv_size_;
      recv_size_ = 0;
      return true;
    } else {
      memmove(recv_buffer_, recv_buffer_ + drop_size_, recv_size_ - drop_size_);
      drop_size_ = 0;
      recv_size_ -= drop_size_;
    }
  }

  while (true) {
    if (recv_size_ >= sizeof(PacketHeader)) {
      PacketHeader *header = reinterpret_cast<PacketHeader *>(recv_buffer_);
      if (header->v == 'V' && header->z == 'Z') {
        uint16_t flag = ntohs(header->flag);
        uint32_t size = ntohl(header->size);
        if (size + sizeof(PacketHeader) > MAX_PACKET_SIZE) {
          LOG_ERROR("Packet size is too large, force drop!");
          drop_size_ = sizeof(PacketHeader) + size;
          drop_size_ -= recv_size_;
          recv_size_ = 0;
          break;
        }

        if (recv_size_ >= size + sizeof(PacketHeader)) {
          // 接收Packet成功，通知读取
          SignalRead(shared_from_this(), flag, recv_buffer_ + sizeof(PacketHeader), size);
          memmove(recv_buffer_, recv_buffer_ + sizeof(PacketHeader) + size, recv_size_ - (sizeof(PacketHeader) + size));
          recv_size_ -= (sizeof(PacketHeader) + size);
        } else {
          break;
        }
      } else {
        LOG_ERROR("Packet error!");
        return false;
      }
    } else {
      break;
    }
  }
  return true;
}
void AsyncPacketSocket::UnInitialize() {
  send_buffers_.clear();

  if (read_event_) {
    event_del(read_event_);
    event_free(read_event_);
    read_event_ = nullptr;
  }

  if (write_event_) {
    event_del(write_event_);
    event_free(write_event_);
    write_event_ = nullptr;
  }

  if (closed_event_) {
    event_del(closed_event_);
    event_free(closed_event_);
    closed_event_ = nullptr;
  }

  SAFE_DELETE_ARRAY(recv_buffer_);
  recv_size_ = 0;

  if (fd_ != INVALID_SOCKET) {
    evutil_closesocket(fd_);
    fd_ = INVALID_SOCKET;
  }
}

void AsyncPacketSocket::EmitError(StpError error) {
  SignalError(shared_from_this(), error);

  UnInitialize();
}

}