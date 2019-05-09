//
// Created by victor on 19-5-7.
//

#include "p2p_client.h"
#include <csignal>
#include <getopt.h>
#include <iostream>

#define VERSION "1.0.0"

namespace vzc {

static const struct option long_options[] = {
    {"name", required_argument, nullptr, 'n'},
    {"type", required_argument, nullptr, 't'},
    {"host", required_argument, nullptr, 'h'},
    {"port", required_argument, nullptr, 'p'},
    {"client", required_argument, nullptr, 'c'},
    {"log", no_argument, nullptr, 'l'},
    {"help", no_argument, nullptr, '?'},
    {"version", no_argument, nullptr, 'v'},
    {nullptr, 0, nullptr, 0}
};

static void PrintUsage() {
  std::cout << "Usage: p2p_client [options] ..." << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -?,--help                    Display this information" << std::endl;
  std::cout << "  -n,--name=name               This client name" << std::endl;
  std::cout << "  -t,--type=nat type           Nat type number" << std::endl;
  std::cout << "  -h,--host=ip                 Server hostname" << std::endl;
  std::cout << "  -p,--port=port               Server port" << std::endl;
  std::cout << "  -c,--client=client name      Target client name" << std::endl;
  std::cout << "  -l,--log                     Write log to file" << std::endl;
  std::cout << "  -v,--version                 Display version" << std::endl;
  std::cout << std::endl << "Copyright © 2019 Vision-Zenith Tech. Co., Ltd. All Rights Reserved." << std::endl;
}

static void PrintVersion() {
  std::cout << "p2p_client version " << VERSION << std::endl;
  std::cout << std::endl << "Copyright © 2019 Vision-Zenith Tech. Co., Ltd. All Rights Reserved." << std::endl;
}

P2PClient::P2PClient()
    : server_port_(0), type_(NT_UNKNOWN) {

}
P2PClient::~P2PClient() {

}
bool P2PClient::Initialize(int argc, char *const *argv) {
  int idx = 0;
  int opt;
  bool log = false;
  while (true) {
    opt = getopt_long(argc, argv, "?n:h:p:t:lv", long_options, &idx);
    if (opt == -1) break;
    switch (opt) {
      case 'n':name_ = optarg;
        break;
      case 't':type_ = (NatType) FromString<int>(optarg);
        break;
      case 'h':server_host_ = optarg;
        break;
      case 'p':server_port_ = FromString<int>(optarg);
        break;
      case 'c':target_name_ = optarg;
        break;
      case 'l':log = true;
        break;
      case 'v':PrintVersion();
        return false;
      case '?':PrintUsage();
        return false;
      default:PrintUsage();
        return false;
    }
  }

  if (name_.empty() || type_ == NT_UNKNOWN || server_host_.empty() || server_port_ == 0) {
    PrintUsage();
    return false;
  }

  if (!InitializeLogger(log, "./logs/client.log")) {
    return false;
  }

  event_loop_.reset(new EventLoop);
  if (!event_loop_->Initialize()) {
    return false;
  }

  signal_handler_.reset(new SignalHandler(event_loop_));
  signal_handler_->SignalEvent.connect(this, &P2PClient::OnSignalEvent);
  if (!signal_handler_->ListenSignal(SIGINT)) {
    LOG_ERROR("Listen INT signal failed!");
    return false;
  }

  listener_.reset(new Listener(event_loop_));
  listener_->SignalAccept.connect(this, &P2PClient::OnListenAccept);
  listener_->SignalError.connect(this, &P2PClient::OnListenError);
  if (!listener_->Start(InetAddr("0.0.0.0", 7001))) {
    InetAddr listen_addr = listener_->GetAddr();
    LOG_INFO_FMT("Listen server on %s!", listen_addr.ToString().c_str());
  } else {
    LOG_ERROR("Listen server failed!");
    return false;
  }

  server_connector_.reset(new Connector(event_loop_));
  server_connector_->SignalConnected.connect(this, &P2PClient::OnServerConnected);
  server_connector_->SignalError.connect(this, &P2PClient::OnServerConnectError);
  if (!server_connector_->Start(InetAddr("47.105.47.51", 7000))) {
    LOG_ERROR("Connect server failed!");
    return false;
  }

  return true;
}
void P2PClient::Run() {
  if (event_loop_) {
    event_loop_->Run();
  }
}
void P2PClient::UnInitialize() {
  if (signal_handler_) {
    signal_handler_.reset();
  }

  if (listener_) {
    listener_.reset();
  }

  if (server_connector_) {
    server_connector_.reset();
  }

  if (event_loop_) {
    event_loop_->UnInitialize();
  }

  UnInitializeLogger();
}
void P2PClient::OnSignalEvent(SignalHandler::Ptr signal_hander, int signal) {
  switch (signal) {
    case SIGINT:LOG_WARN("Catch INT signal, exit client!");
      event_loop_->Stop();
      break;
    default:break;
  }
}
void P2PClient::OnListenAccept(Listener::Ptr listener, evutil_socket_t fd, const InetAddr &addr) {

}
void P2PClient::OnListenError(Listener::Ptr listener) {

}
void P2PClient::OnServerConnected(Connector::Ptr connector, evutil_socket_t fd, const InetAddr &addr) {
  server_socket_.reset(new AsyncPacketSocket(event_loop_, fd, addr));
  server_socket_->SignalRead.connect(this, &P2PClient::OnServerSocketRead);
  server_socket_->SignalError.connect(this, &P2PClient::OnServerSocketError);
  if (!server_socket_->Initialize()) {
    LOG_ERROR("Initialize server socket failed!");
    return;
  }

  LoginServer();
}
void P2PClient::OnServerConnectError(Connector::Ptr connector) {

}
void P2PClient::OnServerSocketRead(AsyncPacketSocket::Ptr socket, uint16_t flag, const char *data, size_t size) {
  if (flag == PKG_FLAG_RESPONSE) {

  }
}
void P2PClient::OnServerSocketError(AsyncPacketSocket::Ptr socket, StpError error) {

}
void P2PClient::LoginServer() {
  Json::Value data;
  data[JKEY_CMD] = JVAL_CMD_LOGIN;
  Json::Value &body = data[JKEY_BODY];
  body["name"] = name_;
  body["type"] = type_;

  server_socket_->SendJson(PKG_FLAG_REQUEST, data);
}
}