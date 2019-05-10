//
// Created by victor on 19-5-7.
//

#ifndef SRC_LIB_COMMON_P2P_SERVER_H_
#define SRC_LIB_COMMON_P2P_SERVER_H_

#include "common/signal_handler.h"
#include "common/listener.h"
#include "common/async_packet_socket.h"

namespace vzc {

struct ClientInfo {
  std::string name;
  int type;
  InetAddr addr;
  AsyncPacketSocket::Ptr socket;
};

class P2PServer : public noncopyable,
                  public sigslot::has_slots<>,
                  public std::enable_shared_from_this<P2PServer> {
 public:
  typedef std::shared_ptr<P2PServer> Ptr;
  P2PServer();
  virtual ~P2PServer();

  bool Initialize(int argc, char *const argv[]);
  void Run();
  void UnInitialize();

 private:
  void OnSignalEvent(SignalHandler::Ptr signal_hander, int signal);

  void OnListenAccept(Listener::Ptr listener, evutil_socket_t fd, const InetAddr& addr);
  void OnListenError(Listener::Ptr listener);

  void OnSocketRead(AsyncPacketSocket::Ptr socket, uint16_t flag, const char *data, size_t size);
  void OnSocketError(AsyncPacketSocket::Ptr socket, StpError error);

  void SendPuchResponse(AsyncPacketSocket::Ptr socket, ClientInfo &info);

 private:
  EventLoop::Ptr event_loop_;
  SignalHandler::Ptr signal_handler_;
  Listener::Ptr listener_;
  std::set<AsyncPacketSocket::Ptr> sockets_;
  std::map<std::string, ClientInfo> clients_;
};

}

#endif //SRC_LIB_COMMON_P2P_SERVER_H_
