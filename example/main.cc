#include <format>
#include <print>
#include <cerrno>
#include <cstring>
#include <string>
#include <http/http.h>

int main() {
  http::loglevel(http::LogLevel::INFO);
  http::Router router;

  router.add("/", http::Method::GET, [](const http::Request& req) {
    return http::Response("Hello, World!");
  });

  router.add("/user/.+$", http::Method::GET, [](const http::Request& req) {
    const auto username = req.segments().back();
    return http::Response(std::format("welcome back {}", username));
  });

  router.add("/id/[0-9]", http::Method::GET, [](const http::Request& req) {
    const auto& id = std::stoi(req.segments().back());
    return http::Response(std::format("user id: {}", id));
  });

  router.add("/hello", http::Method::GET, [](const http::Request& req) {
    std::string response =
      std::format(
       "Hello! {}",
       req.params.contains("name") ? req.params.at("name") : "buddy"
      );

    return http::Response(response);
  });

  http::Server server(router);
  if(server.port(8080).run() < 0) {
    std::println("failed to run server: {}", strerror(errno));
    exit(EXIT_FAILURE);
  }
}
