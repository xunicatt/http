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

  #ifdef THREADPOOL
    explicit Server(const Router& router, const size_t max_workers = std::thread::hardware_concurrency());
  #else
    explicit Server(const Router& router);
  #endif

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
  const Router& m_router;
  uint16_t      m_port;
  int           m_socket;
  sockaddr_in   m_addr;
  socklen_t     m_len;
  std::string   m_addrs;

  #ifdef THREADPOOL
    DynamicThreadPool m_pool;
    std::jthread m_accept_thread;
  #endif
};
}

#endif
