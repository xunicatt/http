#include <http.h>
#include <server.h>
#include <logger.h>
#include <router.h>
#include <threadpool.h>
#include <chrono>
#include <format>
#include <functional>
#include <mutex>
#include <cerrno>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdlib.h>
#include <thread>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>

#ifndef CLIENT_MAX_QUEUE_SIZE
  #define CLIENT_MAX_QUEUE_SIZE 512
#endif

static http::SignalHandler s_signal_handler { .mut = {}, .close_intp = false };
static void signal_handler_func(int);

namespace http {
Server::Server(const Router& router, const size_t max_workers)
: m_router(router),
  m_port(8080),
  m_addrs("0.0.0.0"),
  m_pool(DynamicThreadPool(max_workers))
{
  http::debug("registering signal interupt handler");
  if (signal(SIGINT, signal_handler_func) == SIG_ERR) {
    perror(strerror(errno));
    exit(1);
  }
}

Server& Server::port(const uint16_t& port) {
  m_port = port;
  return *this;
}

Server& Server::addrs(const std::string& vaddrs) {
  m_addrs = vaddrs;
  return *this;
}

uint16_t Server::port() const {
  return m_port;
}

const std::string& Server::addrs() const {
  return m_addrs;
}

int Server::run() {
  info(std::format("libhttp version: {}", http::ver::to_string()));

  int ret = 0;
  const int optval = 1;

  if (m_socket = socket(AF_INET, SOCK_STREAM, 0); m_socket < 0) {
    return m_socket;
  }
  http::debug("created socket");

  if (ret = setsockopt(
    m_socket,
    SOL_SOCKET,
    SO_REUSEADDR,
    &optval,
    sizeof(optval)
  ); ret < 0) {
    return ret;
  }
  http::debug("set socket options");

  int flags = fcntl(m_socket, F_GETFL, 0);
  fcntl(m_socket, F_SETFL, flags | O_NONBLOCK);

  m_addr = (sockaddr_in){
    #if defined(__APPLE__) || defined(__MACH__)
      .sin_len = {},
    #endif
    .sin_family = AF_INET,
    .sin_port = htons(m_port),
    .sin_addr = {
      .s_addr = inet_addr(m_addrs.c_str())
    },
    .sin_zero = {},
  };

  m_len = sizeof(m_addr);

  if (ret = bind(m_socket, (sockaddr*)&m_addr, m_len); ret < 0) {
    return ret;
  }
  http::debug("binded socket to an address");

  if (ret = listen(m_socket, CLIENT_MAX_QUEUE_SIZE); ret < 0) {
    return ret;
  }
  http::debug("started listening on that address");

  http::debug("starting accept loop in a separate thread");
  m_accept_thread = std::jthread([this](std::stop_token stoken) {
    while (!stoken.stop_requested()) {
      sockaddr_in addr = {};
      socklen_t len = sizeof(addr);
      int client = accept(m_socket, (sockaddr*)&addr, &len);

      if (client < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
          continue;
        }

        if (errno == EBADF || errno == EINVAL) {
          http::debug("accept() returned with EBADF/EINVAL");
          return;
        }

        http::error(strerror(errno));
        continue;
      }

      http::debug(std::format("connected to a client: fd: {}", client));
      http::debug("enqueuing new client handler into the queue");
      m_pool.enqueue([router = std::cref(m_router), client = client]() {
        const std::string res = router.get().handle(client);

        write(client, res.c_str(), res.length());
        http::debug("sent response to client");

        if (close(client) < 0) {
          http::error(strerror(errno));
          return;
        }

        http::debug("connection closed with client");
        return;
      });

      http::debug("submitted client to a worker in pool");
    }
  });

  info(std::format("started server on port: {}", m_port));

  while (true) {
    { /* mutex is scoped to call unlock at the end of scope */
      std::lock_guard<std::mutex> guard(s_signal_handler.mut);
      if(s_signal_handler.close_intp) break;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  info("shutting down");
  m_accept_thread.request_stop();

  shutdown(m_socket, SHUT_RDWR);
  close(m_socket);

  if (m_accept_thread.joinable()) {
    m_accept_thread.join();
  }

  m_pool.shutdown();

  return 0;
}
}

static void signal_handler_func(int) {
  http::info("received shutdown signal");
  std::lock_guard<std::mutex> guard(s_signal_handler.mut);
  s_signal_handler.close_intp = true;
}
