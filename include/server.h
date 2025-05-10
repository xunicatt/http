#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include <arpa/inet.h>

#include "router.h"

namespace http {
class Server {
public:
  Server(const Router&);
  Server& port(const uint16_t&);
  Server& addrs(const std::string&);
  int run();

private:
  const Router& router;
  uint16_t _port;
  int _socket;
  sockaddr_in addr;
  socklen_t len;
  std::string _addrs;
};
}

#endif
