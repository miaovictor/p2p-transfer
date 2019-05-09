#include "common/p2p_client.h"

int main(int argc, char *argv[]) {
  vzc::P2PClient::Ptr client(new vzc::P2PClient());
  if(client->Initialize(argc, argv)) {
    client->Run();
  }

  client->UnInitialize();
  return 0;
}