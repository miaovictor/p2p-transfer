//
// Created by victor on 19-3-5.
//

#ifndef SRC_LIB_COMMON_GLOBAL_H
#define SRC_LIB_COMMON_GLOBAL_H

#include <memory.h>
#include "common/logger.h"

#ifdef WIN32
#else
#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif
#endif

//////////////////////////////////////////////////////////////////////////
// Safe release pointer
#ifndef SAFE_DELETE
#define SAFE_DELETE(p) do {if (p != nullptr) {delete p; p = nullptr;}}while(0)
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) do {if (p != nullptr) {delete[] p; p = nullptr;}}while(0)
#endif


#define PKG_FLAG_HEARTBEAT 0
#define PKG_FLAG_REQUEST 1
#define PKG_FLAG_RESPONSE 2

#define JKEY_CMD "cmd"
#define JKEY_BODY "body"

#define JVAL_CMD_LOGIN "login"
#define JVAL_CMD_LOGOUT "logout"
#define JVAL_CMD_PUNCH "punch"


//////////////////////////////////////////////////////////////////////////
// Error
namespace vzc {
enum StpError {
  OK,
  Unknown,
  State,
  Socket_Connect,
  Socket_Closed,
  Socket_Receive,
  Socket_Send,
  Socket_Packet,
  Socket_Init,
  Parse_Json,
  Packet_Flag,
  Packet_Command
};

enum NatType {
  NT_UNKNOWN = 0,
  NT_PUBLIC = 1,
  NT_FULL_CONE_NAT,
  NT_RESTRICTED_CONE_NAT,
  NT_PORT_RESTRICTED_CONE_NAT,
  NT_SYMMETRIC_NAT
};

template<typename T>
static std::string ToString(T t) {
  std::stringstream ss;
  ss << t;
  return ss.str();
}

template<typename T>
static T FromString(const char *str, T val = 0) {
  std::istringstream iss(str);
  T t;
  iss >> t;
  if (iss.fail() || !iss.eof()) {
    return val;
  }
  return t;
}

std::string StringFormat(char const *format, ...);
}

#endif //SRC_LIB_COMMON_GLOBAL_H
