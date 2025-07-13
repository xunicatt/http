#include <cstring>
#include <http.h>

int main() {
  http::Router router;
  router.add("/user", http::Method::Post, [](const http::Request& req) {
    if(!req.params.contains("username")) {
      return http::Response(http::StatusCode::BadRequest);
    }

    return http::Response(std::format("Hi! {}", req.params.at("username")));
  });

  http::Server server(router);
  if(server.port(8080).run() < 0) {
    std::perror(std::strerror(errno));
    std::exit(EXIT_FAILURE);
  }

  return 0;
}
