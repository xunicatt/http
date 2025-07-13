#include <cstring>
#include <http.h>

int main() {
  http::Router router;
  router.add("/", http::Method::Get, [](const http::Request&) {
    return http::Response("Hello, World! using GET");
  });

  router.add("/", http::Method::Post, [](const http::Request&) {
    return http::Response("Hello, World! using POST");
  });

  http::Server server(router);
  if(server.port(8080).run() < 0) {
    std::perror(std::strerror(errno));
    std::exit(EXIT_FAILURE);
  }

  return 0;
}
