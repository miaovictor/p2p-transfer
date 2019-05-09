//
// Created by victor on 19-3-11.
//

#include "data_buffer_pool.h"

namespace vzc {

DataBuffer::DataBuffer(size_t max_buffer_size)
    : data_(nullptr),
      size_(0),
      max_buffer_size_(max_buffer_size) {
  data_ = new char[max_buffer_size];
}

DataBuffer::~DataBuffer() {
  delete[] data_;
}

bool DataBuffer::WriteBytes(const char *data, size_t size) {
  if (size_ + size > max_buffer_size_)
    return false;

  memcpy(data_ + size_, data, size);
  size_ += size;

  return true;
}

bool DataBuffer::WriteString(const char *data) {
  size_t size = strlen(data);
  return WriteBytes(data, size);
}

DataBufferPool::DataBufferPool(size_t max_buffer_size)
    : max_buffer_size_(max_buffer_size) {

}

DataBufferPool::~DataBufferPool() {
  for (auto iter = buffers_.begin(); iter != buffers_.end(); ++iter) {
    DataBuffer *buffer = *iter;
    delete buffer;
  }
}

DataBuffer::Ptr DataBufferPool::TakeDataBuffer() {
  DataBuffer *buffer = nullptr;

  if (buffers_.size()) {
    buffer = buffers_.front();
    buffers_.pop_front();
  } else {
    buffer = new DataBuffer(max_buffer_size_);
  }

  return DataBuffer::Ptr(buffer, [=](DataBuffer *p) {
    p->size_ = 0;
    buffers_.push_back(p);
    LOG_TRACE_FMT("Recycle data buffer! buffer size: %u!", (uint32_t) buffers_.size());
  });
}

}