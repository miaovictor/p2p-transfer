//
// Created by victor on 19-5-7.
//

#include "p2p_server.h"
#include <csignal>

namespace vzc {

P2PServer::P2PServer() {

}
P2PServer::~P2PServer() {

}
bool P2PServer::Initialize(int argc, char *const *argv) {
  if (!InitializeLogger(true, "./logs/server.log")) {
    return false;
  }

  event_loop_.reset(new EventLoop);
  if (!event_loop_->Initialize()) {
    return false;
  }

  signal_handler_.reset(new SignalHandler(event_loop_));
  signal_handler_->SignalEvent.connect(this, &P2PServer::OnSignalEvent);
  if (!signal_handler_->ListenSignal(SIGINT)) {
    LOG_ERROR("Listen INT signal failed!");
    return false;
  }

  listener_.reset(new Listener(event_loop_));
  listener_->SignalAccept.connect(this, &P2PServer::OnListenAccept);
  listener_->SignalError.connect(this, &P2PServer::OnListenError);
  if (listener_->Start(InetAddr("0.0.0.0", 7000))) {
    InetAddr listen_addr = listener_->GetAddr();
    LOG_INFO_FMT("Listen server on %s!", listen_addr.ToString().c_str());
  } else {
    LOG_ERROR("Listen server failed!");
    return false;
  }

  return true;
}
void P2PServer::Run() {
  if (event_loop_) {
    event_loop_->Run();
  }
}
void P2PServer::UnInitialize() {
  if (signal_handler_) {
    signal_handler_.reset();
  }

  if (listener_) {
    listener_.reset();
  }

  if (event_loop_) {
    event_loop_->UnInitialize();
  }

  UnInitializeLogger();
}

void P2PServer::OnSignalEvent(SignalHandler::Ptr signal_hander, int signal) {
  switch (signal) {
    case SIGINT:LOG_WARN("Catch INT signal, exit server!");
      event_loop_->Stop();
      break;
    default:break;
  }
}

void P2PServer::OnListenAccept(Listener::Ptr listener, evutil_socket_t fd, const InetAddr &addr) {
  AsyncPacketSocket::Ptr socket(new AsyncPacketSocket(event_loop_, fd, addr));
  socket->SignalRead.connect(this, &P2PServer::OnSocketRead);
  socket->SignalError.connect(this, &P2PServer::OnSocketError);
  if (socket->Initialize()) {
    sockets_.insert(socket);
  }
}

void P2PServer::OnListenError(Listener::Ptr listener) {
  LOG_ERROR("Listener error!");
}

void P2PServer::OnSocketRead(AsyncPacketSocket::Ptr socket, uint16_t flag, const char *data, size_t size) {
  if (flag == PKG_FLAG_HEARTBEAT) {
    socket->Send(PKG_FLAG_HEARTBEAT, nullptr, 0);
  } else if (flag == PKG_FLAG_REQUEST) {
    Json::CharReaderBuilder builder;
    Json::Value request;
    Json::CharReader *reader = builder.newCharReader();
    JSONCPP_STRING error_string;
    if (!reader->parse(data, data + size, &request, &error_string)) {
      LOG_ERROR_FMT("Parse request to json failed! error: %s", error_string.c_str());
      return;
    }

    std::string cmd = request[JKEY_CMD].asString();
    if (cmd == JVAL_CMD_LOGIN) {
      Json::Value &body = request[JKEY_BODY];

      ClientInfo info;
      info.addr = socket->GetAddr();
      info.name = body["name"].asString();
      info.type = body["type"].asInt();
      info.socket = socket;

      clients_.insert(std::make_pair(info.name, info));

      LOG_INFO_FMT("Client [%s] login! %s", info.name.c_str(), info.addr.ToString().c_str());

      Json::Value response;
      response[JKEY_CMD] = JVAL_CMD_LOGIN;
      response[JKEY_STATUS] = 0;

      socket->SendJson(PKG_FLAG_RESPONSE, response);
    } else if (cmd == JVAL_CMD_LOGOUT) {
      Json::Value &body = request[JKEY_BODY];

      std::string name = body["name"].asString();
      clients_.erase(name);

      Json::Value response;
      response[JKEY_CMD] = JVAL_CMD_LOGOUT;
      response[JKEY_STATUS] = 0;

      socket->SendJson(PKG_FLAG_RESPONSE, response);
    } else if (cmd == JVAL_CMD_PUNCH) {
      Json::Value &body = request[JKEY_BODY];
      std::string name = body["name"].asString();
      std::string target_name = body["target_name"].asString();

      auto iter = clients_.find(name);
      auto target_iter = clients_.find(target_name);
      if(iter == clients_.end() || target_iter == clients_.end()) {
        Json::Value response;
        response[JKEY_CMD] = JVAL_CMD_PUNCH;
        response[JKEY_STATUS] = 1;

        socket->SendJson(PKG_FLAG_RESPONSE, response);
      } else {
        SendPuchResponse(target_iter->second.socket, iter->second);
        SendPuchResponse(iter->second.socket, target_iter->second);
      }
    }
  }
}

void P2PServer::OnSocketError(AsyncPacketSocket::Ptr socket, StpError error) {
  LOG_ERROR_FMT("Socket error! %d", error);
}
void P2PServer::SendPuchResponse(AsyncPacketSocket::Ptr socket, ClientInfo &info) {
  Json::Value response;
  response[JKEY_CMD] = JVAL_CMD_PUNCH;
  response[JKEY_STATUS] = 0;
  Json::Value &body = response[JKEY_BODY];
  body["name"] = info.name;
  body["type"] = info.type;
  body["host"] = info.addr.GetIPString();
  body["port"] = info.addr.GetPort();

  socket->SendJson(PKG_FLAG_RESPONSE, response);
}

}
