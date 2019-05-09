//
// Created by victor on 19-3-6.
//

#ifndef SRC_LIB_COMMON_NONCOPYABLE_H
#define SRC_LIB_COMMON_NONCOPYABLE_H

namespace vzc {

class noncopyable {
 protected:
  constexpr noncopyable() = default;
  ~noncopyable() = default;

  noncopyable(const noncopyable&) = delete;
  noncopyable& operator=(const noncopyable&) = delete;
};

}

#endif //SRC_LIB_COMMON_NONCOPYABLE_H