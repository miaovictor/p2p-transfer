//
// Created by victor on 19-3-12.
//

#ifndef SRC_LIB_COMMON_INET_ADDR_H
#define SRC_LIB_COMMON_INET_ADDR_H

#include "common/global.h"
#include "event2/event.h"

namespace vzc {

class InetAddr {
 public:
  InetAddr();
  InetAddr(const InetAddr &addr);
  InetAddr(const struct sockaddr_in &addr);
  InetAddr(uint32_t ip, uint16_t port);
  InetAddr(const char *ip, uint16_t port);
  virtual ~InetAddr();

 public:
  bool IsNull() const;
  InetAddr &operator=(const InetAddr &addr);
  InetAddr &operator=(const struct sockaddr_in &addr);
  InetAddr &operator=(const std::string &addr); // ip:port

  void SetIP(uint32_t ip);
  void SetIPString(const char *ip);
  void SetPort(uint16_t port);

  uint32_t GetIP() const;
  const std::string GetIPString() const;
  uint16_t GetPort() const;

  struct sockaddr_in *GetAddr();
  const struct sockaddr_in *GetAddr() const;

  const std::string ToString() const;

  bool operator==(const InetAddr &addr) const;
  bool operator!=(const InetAddr &addr) const;

 private:
  struct sockaddr_in addr_;
};

}

#endif //SRC_LIB_COMMON_INET_ADDR_H
