//
// Created by victor on 19-3-12.
//

#include "inet_addr.h"
#include <memory.h>

namespace vzc {

InetAddr::InetAddr() {
  memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = htonl(INADDR_ANY);
  addr_.sin_port = htons(0);
}

InetAddr::InetAddr(const InetAddr &addr) {
  addr_ = addr.addr_;
}

InetAddr::InetAddr(const struct sockaddr_in &addr) {
  addr_ = addr;
}

InetAddr::InetAddr(uint32_t ip, uint16_t port) {
  memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  SetIP(ip);
  SetPort(port);
}

InetAddr::InetAddr(const char *ip, uint16_t port) {
  memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  SetIPString(ip);
  SetPort(port);
}

InetAddr::~InetAddr() {

}

bool InetAddr::IsNull() const {
  return (ntohs(addr_.sin_port) == 0);
}

InetAddr &InetAddr::operator=(const InetAddr &addr) {
  addr_ = addr.addr_;
  return *this;
}

InetAddr &InetAddr::operator=(const struct sockaddr_in &addr) {
  addr_ = addr;
  return *this;
}

InetAddr &InetAddr::operator=(const std::string &addr) {
  std::string::size_type pos = addr.find(':');
  if (std::string::npos == pos) {
    return *this;
  }

  SetPort(static_cast<uint16_t>(strtoul(addr.substr(pos + 1).c_str(), nullptr, 10)));
  SetIPString(addr.substr(0, pos).c_str());

  return *this;
}

void InetAddr::SetIP(uint32_t ip) {
  addr_.sin_addr.s_addr = htonl(ip);
}

void InetAddr::SetIPString(const char *ip) {
  evutil_inet_pton(AF_INET, ip, &addr_.sin_addr);
}

void InetAddr::SetPort(uint16_t port) {
  addr_.sin_port = htons(port);
}

uint32_t InetAddr::GetIP() const {
  return ntohl(addr_.sin_addr.s_addr);
}

const std::string InetAddr::GetIPString() const {
  char ip[32] = {0};
  evutil_inet_ntop(AF_INET, &(addr_.sin_addr), ip, sizeof(ip));
  return ip;
}

uint16_t InetAddr::GetPort() const {
  return ntohs(addr_.sin_port);
}

struct sockaddr_in *InetAddr::GetAddr() {
  return &addr_;
}

const struct sockaddr_in *InetAddr::GetAddr() const {
  return &addr_;
}

bool InetAddr::operator==(const InetAddr &addr) const {
  return (ntohl(addr_.sin_addr.s_addr) == ntohl(addr.addr_.sin_addr.s_addr)
      && ntohs(addr_.sin_port) == ntohs(addr.addr_.sin_port));
}

bool InetAddr::operator!=(const InetAddr &addr) const {
  return !((*this) == addr);
}

const std::string InetAddr::ToString() const {
  return GetIPString() + ":" + vzc::ToString<uint16_t>(GetPort());
}



}