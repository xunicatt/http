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

#ifndef CLIENT_MAX_QUEUE_SIZE
  #define CLIENT_MAX_QUEUE_SIZE 512
#endif

static http::SignalHandler signal_handler { .mut = {}, .close_intp = false };
static void signal_handler_func(int);

#ifndef THREADPOOL
  static void client_handler_func_multi_threaded(const http::Router&,const int);
  static void client_handler_func(const http::Router&,int&,sockaddr*,socklen_t*);
#endif

namespace http {
Server::Server(const Router& router, const size_t max_workers)
: router(router),
  _port(8080),
  _addrs("0.0.0.0")
#ifdef THREADPOOL
  , pool(DynamicThreadPool(max_workers))
#endif
{
  http::debug("registering signal interupt handler");
  if (signal(SIGINT, signal_handler_func) == SIG_ERR) {
    perror(strerror(errno));
    exit(1);
  }
}

Server& Server::port(const uint16_t& port) {
  _port = port;
  return *this;
}

Server& Server::addrs(const std::string& vaddrs) {
  _addrs = vaddrs;
  return *this;
}

uint16_t Server::port() const {
  return _port;
}

const std::string& Server::addrs() const {
  return _addrs;
}

int Server::run() {
  info(std::format("libhttp version: {}", http::ver::to_string()));
  #ifdef THREADPOOL
    info("using threadpool");
  #endif

  int ret = 0;
  const int optval = 1;

  if (_socket = socket(AF_INET, SOCK_STREAM, 0); _socket < 0) {
    return _socket;
  }
  http::debug("created socket");

  if (ret = setsockopt(
    _socket,
    SOL_SOCKET,
    SO_REUSEADDR,
    &optval,
    sizeof(optval)
  ); ret < 0) {
    return ret;
  }
  http::debug("set socket options");

  addr = (sockaddr_in){
    #if defined(__APPLE__) || defined(__MACH__)
      .sin_len = {},
    #endif
    .sin_family = AF_INET,
    .sin_port = htons(_port),
    .sin_addr = {
      .s_addr = inet_addr(_addrs.c_str())
    },
    .sin_zero = {},
  };

  len = sizeof(addr);

  if (ret = bind(_socket, (sockaddr*)&addr, len); ret < 0) {
    return ret;
  }
  http::debug("binded socket to an address");

  if (ret = listen(_socket, CLIENT_MAX_QUEUE_SIZE); ret < 0) {
    return ret;
  }
  http::debug("started listening on that address");

  #ifndef THREADPOOL
    http::debug("started main client hanlder in a new thread");
    std::thread client_hanlder(
      client_handler_func,
      std::cref(router),
      std::ref(_socket),
      (sockaddr*)&addr,
      &len
    );
  #else
    http::debug("starting accept loop in a separate thread");
    std::jthread accept_thread([this](std::stop_token stoken) {
      while (!stoken.stop_requested()) {
        sockaddr_in addr = {};
        socklen_t len = sizeof(addr);
        int client = accept(_socket, (sockaddr*)&addr, &len);
        http::debug("connected to a client");

        if (client < 0) {
          if (errno == EBADF || errno == EINVAL) {
            return;
          }

          http::error(strerror(errno));
          continue;
        }


        http::debug("enqueuing new client handler into the queue");
        pool.enqueue([this, client = client]() {
          const std::string res = router.handle(client);
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
  #endif

  info(std::format("started server on port: {}", _port));

  while (true) {
    { /* mutex is scoped to call unlock at the end of scope */
      std::lock_guard<std::mutex> guard(signal_handler.mut);
      if(signal_handler.close_intp) break;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  info("shutting down");
  if (ret = shutdown(_socket, SHUT_RDWR); ret < 0) {
    return ret;
  }

  if (ret = close(_socket); ret < 0) {
    return ret;
  }

  #ifndef THREADPOOL
    client_hanlder.join();
  #else
    pool.shutdown();
  #endif

  return 0;
}
}

static void signal_handler_func(int) {
  http::info("received shutdown signal");
  std::lock_guard<std::mutex> guard(signal_handler.mut);
  signal_handler.close_intp = true;
}

#ifndef THREADPOOL
  void client_handler_func_multi_threaded(
    const http::Router& router,
    const int client
  ) {
    const std::string res = router.handle(client);
    write(client, res.c_str(), res.length());
    http::debug("sent response to client");

    if (close(client) < 0) {
      http::error(strerror(errno));
      return;
    }

    http::debug("connection closed with client");
    return;
  }

  void client_handler_func(
    const http::Router& router,
    int& sock,
    sockaddr* addr,
    socklen_t* len
  ) {
    while (true) {
      int client = accept(sock, addr, len);
      http::debug("connected to a client");

      if (client < 0) {
        if (errno == EBADF || errno == EINVAL) {
          return;
        }

        http::error(strerror(errno));
        continue;
      }

      http::debug("launched sub client handler in a new thread");
      std::thread client_handler(
        client_handler_func_multi_threaded,
        std::cref(router),
        client
      );
      client_handler.detach();
    }
  }
#endif
