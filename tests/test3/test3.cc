#include <cstring>
#include <lime/lime.h>

int main() {
  namespace http = lime::http;

  http::Router router;
  router.add_regex("/user/[0-9]", http::Method::Get, [](const http::Request& req) {
    const auto id = req.segments().back();
    return http::Response(std::format("User: {}", id));
  });

  http::Server server(router);
  if(server.port(8080).run() < 0) {
    std::perror(std::strerror(errno));
    std::exit(EXIT_FAILURE);
  }

  return 0;
}
