#include <cstring>
#include <print>
#include <lime.h>

int main() {
  namespace http = lime::http;

  http::Router router;
  router.add("/", http::Method::Get, [](const http::Request&){
    return http::Response("Hello, World!");
  });

  http::Server server(router);
  if(server.port(8080).run() < 0) {
    std::println("failed to run server: {}", strerror(errno));
    exit(EXIT_FAILURE);
  }
}
