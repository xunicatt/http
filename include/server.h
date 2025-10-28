#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include <thread>
#include <arpa/inet.h>

#include "router.h"
#include "threadpool.h"

namespace http {
class Server {
public:
  /*
  * @brief Create a new server from router.
  * @param router Instance of http::Router.
  */
  Server(const Router& router, const size_t max_threads = std::thread::hardware_concurrency());

  /*
  * @brief Set server port.
  * @param port Port.
  */
  Server& port(const uint16_t& port);

  /*
  * @brief Set server address.
  * @param addrs Address.
  */
  Server& addrs(const std::string& addrs);

  /*
  * @brief Get server port.
  * @return Server port.
  */
  [[nodiscard]]
  uint16_t port() const;

  /*
  * @brief Get server address.
  * @return Server Address.
  */
  [[nodiscard]]
  const std::string& addrs() const;

  /*
  * @brief Starts the server.
  * @return Returns 0 on success or negative number on error, check errno for more details.
  */
  [[nodiscard]]
  int run();

private:
  const Router& router;
  uint16_t      _port;
  int           _socket;
  sockaddr_in   addr;
  socklen_t     len;
  std::string   _addrs;
  DynamicThreadPool pool;
};
}

#endif
