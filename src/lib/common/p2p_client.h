//
// Created by victor on 19-5-7.
//

#ifndef SRC_LIB_COMMON_P2P_CLIENT_H_
#define SRC_LIB_COMMON_P2P_CLIENT_H_

#include "common/signal_handler.h"
#include "common/listener.h"
#include "common/connector.h"
#include "common/async_packet_socket.h"

namespace vzc {

class P2PClient : public noncopyable,
                  public sigslot::has_slots<>,
                  public std::enable_shared_from_this<P2PClient> {
 public:
  typedef std::shared_ptr<P2PClient> Ptr;
  P2PClient();
  virtual ~P2PClient();

  bool Initialize(int argc, char *const argv[]);
  void Run();
  void UnInitialize();

 private:
  void OnSignalEvent(SignalHandler::Ptr signal_hander, int signal);

  void OnListenAccept(Listener::Ptr listener, evutil_socket_t fd, const InetAddr& addr);
  void OnListenError(Listener::Ptr listener);

  void OnServerConnected(Connector::Ptr connector, evutil_socket_t fd, const InetAddr& addr);
  void OnServerConnectError(Connector::Ptr connector);
  void OnServerSocketRead(AsyncPacketSocket::Ptr socket, uint16_t flag, const char *data, size_t size);
  void OnServerSocketError(AsyncPacketSocket::Ptr socket, StpError error);

 private:
  void LoginServer();

 private:
  EventLoop::Ptr event_loop_;
  SignalHandler::Ptr signal_handler_;
  Listener::Ptr listener_;
  Connector::Ptr server_connector_;
  AsyncPacketSocket::Ptr server_socket_;

 private:
  std::string name_;
  NatType type_;
  std::string server_host_;
  int server_port_;
  std::string target_name_;
};

}

#endif //SRC_LIB_COMMON_P2P_CLIENT_H_
