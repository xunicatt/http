#include <format>
#include <cerrno>
#include <print>
#include <cstring>
#include <http.h>

int main() {
  // set log level
  http::loglevel(http::LogLevel::INFO);
  http::Router router;

  router.add("/", http::Method::GET, [](const http::Request& req) {
    return http::Response("Hello, World!");
  });

  // routes with pattern matching using regex
  router.add("/user/.+$", http::Method::GET, [](const http::Request& req) {
    // get the last segment in the url
    // /user/..../<username>
    const auto username = req.segments().back();
    return http::Response(std::format("welcome back {}", username));
  });

  router.add("/id/[0-9]", http::Method::GET, [](const http::Request& req) {
    // get the last segment in the url
    // /id/..../<id>
    const auto& id = std::stoi(req.segments().back());
    return http::Response(std::format("user id: {}", id));
  });

  router.add("/hello", http::Method::GET, [](const http::Request& req) {
    std::string response =
      std::format(
       "Hello! {}",
       // get the parameters if exists
       // /hello?name=<name>
       req.params.contains("name") ? req.params.at("name") : "buddy"
      );

    return http::Response(response);
  });

  router.add("/body", http::Method::POST, [](const http::Request& req) {
    // get the body
    return http::Response(req.body.length() == 0 ? "no body??" : req.body);
  });

  http::Server server(router);
  if(server.port(8080).run() < 0) {
    std::println("failed to run server: {}", strerror(errno));
    exit(EXIT_FAILURE);
  }
}
