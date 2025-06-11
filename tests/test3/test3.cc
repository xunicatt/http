#include <cstring>
#include <http/http.h>

int main() {
  http::Router router;
  router.add_regex("/user/[0-9]", http::Method::GET, [](const http::Request& req) {
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
