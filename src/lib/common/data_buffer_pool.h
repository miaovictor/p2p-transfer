//
// Created by victor on 19-3-11.
//

#ifndef SRC_LIB_COMMON_DATA_BUFFER_POOL_H
#define SRC_LIB_COMMON_DATA_BUFFER_POOL_H

#include "common/global.h"
#include "common/noncopyable.hpp"
#include <list>

namespace vzc {

class DataBuffer : public noncopyable,
                   public std::enable_shared_from_this<DataBuffer> {
 public:
  typedef std::shared_ptr<DataBuffer> Ptr;
  DataBuffer(size_t max_buffer_size);
  virtual ~DataBuffer();

  bool WriteBytes(const char* data, size_t size);
  bool WriteString(const char* data);

  char *data_;
  size_t size_;

 private:
  size_t max_buffer_size_;
};

class DataBufferPool : public noncopyable,
                       public std::enable_shared_from_this<DataBufferPool> {
 public:
  typedef std::shared_ptr<DataBufferPool> Ptr;
  DataBufferPool(size_t max_buffer_size);
  virtual ~DataBufferPool();

 public:
  DataBuffer::Ptr TakeDataBuffer();

 private:
  std::list<DataBuffer *> buffers_;
  size_t max_buffer_size_;
};

}

#endif //SRC_LIB_COMMON_DATA_BUFFER_POOL_H
