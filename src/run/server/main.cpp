#include "common/p2p_server.h"

int main(int argc, char *argv[]) {
  vzc::P2PServer::Ptr server(new vzc::P2PServer());
  if(server->Initialize(argc, argv)) {
    server->Run();
  }

  server->UnInitialize();
  return 0;
}