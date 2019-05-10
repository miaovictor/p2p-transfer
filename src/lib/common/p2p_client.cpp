//
// Created by victor on 19-5-7.
//

#include "p2p_client.h"
#include <csignal>
#include <getopt.h>
#include <iostream>
#include <unistd.h>

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
    : server_port_(0),
      type_(NT_UNKNOWN),
      target_connected_(false),
      local_addr_("0.0.0.0", 7001){

}
P2PClient::~P2PClient() {

}
bool P2PClient::Initialize(int argc, char *const *argv) {
  int idx = 0;
  int opt;
  bool log = false;
  while (true) {
    opt = getopt_long(argc, argv, "?n:t:h:p:c:lv", long_options, &idx);
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
  if (listener_->Start(local_addr_, -1)) {
    InetAddr listen_addr = listener_->GetAddr();
    LOG_INFO_FMT("Listen server on %s!", listen_addr.ToString().c_str());
  } else {
    LOG_ERROR("Listen server failed!");
    return false;
  }

  server_connector_.reset(new Connector(event_loop_));
  server_connector_->SignalConnected.connect(this, &P2PClient::OnServerConnected);
  server_connector_->SignalError.connect(this, &P2PClient::OnServerConnectError);
  if (!server_connector_->Start(InetAddr(server_host_.c_str(), server_port_))) {
    LOG_ERROR("Connect server failed!");
    return false;
  }

  server_timer_.reset(new EventTimer(event_loop_));
  server_timer_->SignalTimer.connect(this, &P2PClient::OnServerTimer);
  if (!server_timer_->Initialize(false)) {
    LOG_ERROR("Initialize server timer failed!");
    return false;
  }

  target_timer_.reset(new EventTimer(event_loop_));
  target_timer_->SignalTimer.connect(this, &P2PClient::OnTargetTimer);
  if (!target_timer_->Initialize(false)) {
    LOG_ERROR("Initialize target timer failed!");
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
//  if(target_connected_) {
//    LOG_WARN("Target already connected!");
//    return;
//  }
//
//  LOG_INFO("Accept target connect!");
//  target_connected_ = true;
//
//  target_socket_.reset(new AsyncPacketSocket(event_loop_, fd, addr));
//  target_socket_->SignalRead.connect(this, &P2PClient::OnTargetSocketRead);
//  target_socket_->SignalError.connect(this, &P2PClient::OnTargetSocketError);
//  if (!target_socket_->Initialize()) {
//    LOG_ERROR("Initialize target socket failed!");
//    target_socket_.reset();
//    return;
//  }
//
//  SendTargetRequset();
  LOG_INFO("Client listener callback!");
}
void P2PClient::OnListenError(Listener::Ptr listener) {

}
void P2PClient::OnServerConnected(Connector::Ptr connector, evutil_socket_t fd, const InetAddr &addr) {
  LOG_INFO("Connect server succeed!");
  server_connector_.reset();

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
  LOG_ERROR("Server connect error!");
}
void P2PClient::OnServerSocketRead(AsyncPacketSocket::Ptr socket, uint16_t flag, const char *data, size_t size) {
  if (flag == PKG_FLAG_HEARTBEAT) {
    server_timer_->Start(10 * 1000);
  } else if (flag == PKG_FLAG_RESPONSE) {
    Json::CharReaderBuilder builder;
    Json::Value response;
    Json::CharReader *reader = builder.newCharReader();
    JSONCPP_STRING error_string;
    if (!reader->parse(data, data + size, &response, &error_string)) {
      LOG_ERROR_FMT("Parse response to json failed! error: %s", error_string.c_str());
      return;
    }

    std::string cmd = response[JKEY_CMD].asString();
    if (cmd == JVAL_CMD_LOGIN) {
      int status = response[JKEY_STATUS].asInt();
      if (status == 0) {
        StartServerTimer();

        LOG_INFO("Login server succeed!");

        if (!target_name_.empty()) {
          StartPunchHole();
        }
      }
    } else if (cmd == JVAL_CMD_PUNCH) {
      int status = response[JKEY_STATUS].asInt();
      if (status != 0) {
        LOG_ERROR("Query target info failed!");
        return;
      }

      Json::Value &body = response[JKEY_BODY];
      std::string target_name = body["name"].asString();
      int target_type = body["type"].asInt();
      std::string target_host = body["host"].asString();
      int target_port = body["port"].asInt();

      InetAddr target_addr(target_host.c_str(), (uint16_t) target_port);

      target_info_.name = target_name;
      target_info_.type = target_type;
      target_info_.addr = target_addr;

      ConnectTarget();
    }
  }
}
void P2PClient::OnServerSocketError(AsyncPacketSocket::Ptr socket, StpError error) {
  LOG_ERROR_FMT("Server socket error! %d", error);
}
void P2PClient::LoginServer() {
  Json::Value data;
  data[JKEY_CMD] = JVAL_CMD_LOGIN;
  Json::Value &body = data[JKEY_BODY];
  body["name"] = name_;
  body["type"] = type_;

  server_socket_->SendJson(PKG_FLAG_REQUEST, data);
}
void P2PClient::StartServerTimer() {
  server_socket_->Send(PKG_FLAG_HEARTBEAT, nullptr, 0);
}
void P2PClient::OnServerTimer(EventTimer::Ptr timer) {
  StartServerTimer();
}
void P2PClient::StartPunchHole() {
  Json::Value data;
  data[JKEY_CMD] = JVAL_CMD_PUNCH;
  Json::Value &body = data[JKEY_BODY];
  body["name"] = name_;
  body["target_name"] = target_name_;

  server_socket_->SendJson(PKG_FLAG_REQUEST, data);
}
void P2PClient::ConnectTarget() {
//  target_connector_.reset(new Connector(event_loop_));
//  target_connector_->SignalConnected.connect(this, &P2PClient::OnTargetConnected);
//  target_connector_->SignalError.connect(this, &P2PClient::OnTargetConnectError);
//  if (!target_connector_->Start(target_info_.addr)) {
//  } else {
//    LOG_ERROR("Connect target failed! try again...");
//    target_timer_->Start(100);
//  }

  socklen_t addr_len = sizeof(local_addr_);

  int fd = INVALID_SOCKET;
  int enable = 1;

  LOG_ERROR_FMT("Try connect target [%s], %s", target_info_.name.c_str(), target_info_.addr.ToString().c_str());

  fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (INVALID_SOCKET == fd) {
    LOG_ERROR("Create socket failed!");
    goto NEXT;
  }

  if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable))) {
    LOG_ERROR("Set SO_REUSEADDR failed!");
    goto NEXT;
  }

  if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable))) {
    LOG_ERROR("Set SO_REUSEPORT failed!");
    goto NEXT;
  }

  if (-1 == setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable))) {
    LOG_ERROR("Set SO_KEEPALIVE failed!");
    goto NEXT;
  }

  if (-1 == bind(fd, (const struct sockaddr *) local_addr_.GetAddr(), addr_len)) {
    LOG_ERROR("Bind local address failed!");
    goto NEXT;
  }

  if (-1 == connect(fd, (const struct sockaddr *) target_info_.addr.GetAddr(), addr_len)) {
    LOG_ERROR_FMT("Connect target failed! errno: %d", errno);
    if (errno != ETIMEDOUT) {
      sleep(20);
    }
    goto NEXT;
  }

  LOG_INFO("Punch succeed!");

  target_connected_ = true;

  target_socket_.reset(new AsyncPacketSocket(event_loop_, fd, target_info_.addr));
  target_socket_->SignalRead.connect(this, &P2PClient::OnTargetSocketRead);
  target_socket_->SignalError.connect(this, &P2PClient::OnTargetSocketError);
  if (!target_socket_->Initialize()) {
    LOG_ERROR("Initialize target socket failed!");
    target_socket_.reset();
    return;
  }

  SendTargetRequset();
  return;

NEXT:
  if (fd != INVALID_SOCKET) {
    close(fd);
  }

  target_timer_->Start(1000);
}
void P2PClient::OnTargetConnected(Connector::Ptr connector, evutil_socket_t fd, const InetAddr &addr) {
  LOG_INFO("Connect target succeed!");
  target_connected_ = true;

  target_socket_.reset(new AsyncPacketSocket(event_loop_, fd, addr));
  target_socket_->SignalRead.connect(this, &P2PClient::OnTargetSocketRead);
  target_socket_->SignalError.connect(this, &P2PClient::OnTargetSocketError);
  if (!target_socket_->Initialize()) {
    LOG_ERROR("Initialize target socket failed!");
    target_socket_.reset();
    return;
  }

  SendTargetRequset();
}
void P2PClient::OnTargetConnectError(Connector::Ptr connector) {
  LOG_ERROR("Connect target failed! try again...");
  target_timer_->Start(100);
}
void P2PClient::OnTargetTimer(EventTimer::Ptr timer) {
  if (!target_connected_) {
    ConnectTarget();
  }
}
void P2PClient::OnTargetSocketRead(AsyncPacketSocket::Ptr socket, uint16_t flag, const char *data, size_t size) {
  if (flag == PKG_FLAG_REQUEST) {
    Json::CharReaderBuilder builder;
    Json::Value request;
    Json::CharReader *reader = builder.newCharReader();
    JSONCPP_STRING error_string;
    if (!reader->parse(data, data + size, &request, &error_string)) {
      LOG_ERROR_FMT("Parse request to json failed! error: %s", error_string.c_str());
      return;
    }

    std::string cmd = request[JKEY_CMD].asString();
    if (cmd == JVAL_CMD_TALK) {
      Json::Value &body = request[JKEY_BODY];
      LOG_INFO_FMT("[%s]: %s", target_info_.name.c_str(), body["message"].asCString());
      SendTargetResponse();
    }
  } else if (flag == PKG_FLAG_RESPONSE) {
    Json::CharReaderBuilder builder;
    Json::Value response;
    Json::CharReader *reader = builder.newCharReader();
    JSONCPP_STRING error_string;
    if (!reader->parse(data, data + size, &response, &error_string)) {
      LOG_ERROR_FMT("Parse response to json failed! error: %s", error_string.c_str());
      return;
    }

    std::string cmd = response[JKEY_CMD].asString();
    if (cmd == JVAL_CMD_TALK) {
      Json::Value &body = response[JKEY_BODY];
      LOG_INFO_FMT("[%s]: %s", target_info_.name.c_str(), body["message"].asCString());

      target_timer_->Start(2 * 1000);
    }
  }
}
void P2PClient::OnTargetSocketError(AsyncPacketSocket::Ptr socket, StpError error) {

}
void P2PClient::SendTargetRequset() {
  if (!target_name_.empty()) {
    Json::Value data;
    data[JKEY_CMD] = JVAL_CMD_TALK;
    Json::Value &body = data[JKEY_BODY];
    body["message"] = "Nice to meet you!";

    target_socket_->SendJson(PKG_FLAG_REQUEST, data);

    LOG_INFO_FMT("[%s]: %s", name_.c_str(), body["message"].asCString());
  }
}
void P2PClient::SendTargetResponse() {
  Json::Value data;
  data[JKEY_CMD] = JVAL_CMD_TALK;
  Json::Value &body = data[JKEY_BODY];
  body["message"] = "Nice to meet you, too!";

  target_socket_->SendJson(PKG_FLAG_RESPONSE, data);
  LOG_INFO_FMT("[%s]: %s", name_.c_str(), body["message"].asCString());
}

}