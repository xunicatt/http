#include "http.h"
#include <server.h>
#include <logger.h>
#include <router.h>
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

static http::SignalHandler signal_handler { .close_intp = false };
static void signal_handler_func(int);
static void client_handler_func_multi_threaded(const http::Router&,const int);
static void client_handler_func(const http::Router&,int&,sockaddr*,socklen_t*);

namespace http {
Server::Server(const Router& router)
: router(router), _port(8080), _addrs("0.0.0.0") {
  http::debug("registering signal interupt handler");
  if(signal(SIGINT, signal_handler_func) == SIG_ERR) {
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
  info(std::format("libhttp version: {}", version()));

  int ret = 0;
  const int optval = 1;

  if(_socket = socket(AF_INET, SOCK_STREAM, 0); _socket < 0) {
    return _socket;
  }
  http::debug("created socket");

  if(ret = setsockopt(
    _socket,
    SOL_SOCKET,
    SO_REUSEADDR,
    &optval,
    sizeof(optval)
  ); ret < 0) {
    return ret;
  }
  http::debug("set socket options");

  addr = {
    .sin_family = AF_INET,
    .sin_port = htons(_port),
    .sin_addr = {
      .s_addr = inet_addr(_addrs.c_str())
    }
  };

  len = sizeof(addr);

  if(ret = bind(_socket, (sockaddr*)&addr, len); ret < 0) {
    return ret;
  }
  http::debug("binded socket to an address");

  if(ret = listen(_socket, 10); ret < 0) {
    return ret;
  }
  http::debug("started listening on that address");

  http::debug("started main client hanlder in a new thread");
  std::thread client_hanlder(
    client_handler_func,
    std::cref(router),
    std::ref(_socket),
    (sockaddr*)&addr,
    &len
  );

  info(std::format("started server on port: {}", _port));

  while(true) {
    {
      std::lock_guard<std::mutex> guard(signal_handler.mut);
      if(signal_handler.close_intp) break;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  info("shutting down");
  if(ret = shutdown(_socket, SHUT_RDWR); ret < 0) {
    return ret;
  }

  if(ret = close(_socket); ret < 0) {
    return ret;
  }

  client_hanlder.join();
  return 0;
}
}

static void signal_handler_func(int) {
  http::info("received shutdown signal");
  std::lock_guard<std::mutex> guard(signal_handler.mut);
  signal_handler.close_intp = true;
}

static void client_handler_func_multi_threaded(
  const http::Router& router,
  const int client
) {
  const std::string res = router.handle(client);
  write(client, res.c_str(), res.length());
  http::debug("sent response to client");

  if(close(client) < 0) {
    http::error(strerror(errno));
    return;
  }

  http::debug("connection closed with client");
  return;
}

static void client_handler_func(
  const http::Router& router,
  int& sock,
  sockaddr* addr,
  socklen_t* len
) {
  while(true) {
    int client = accept(sock, addr, len);
    http::debug("connected to a client");

    if(client < 0) {
      if(errno == EBADF || errno == EINVAL) {
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
