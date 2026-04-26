#include <print>
#include <cstring>
#include <http.h>

int main() {
  namespace json = http::json;

  const auto null = json::Object{
    { "my-echo", {} }
  };

  http::Router router;
  router.add("/echo", http::Method::Get, [&null](const http::Request& req) {
    const auto node = json::decode(req.body);
    if (!node) {
      return http::Response(json::encode(null));
    }

    const auto root = node->get<json::Object>();
    if (!root.contains("echo")) {
      return http::Response(json::encode(null));
    }

    const auto resp = json::Object{
      { "my-echo", root.at("echo") },
    };

    return http::Response(json::encode(resp));
  });

  http::Server server{ router };

  if (server.run() < 0) {
    std::println("ERROR: {}", strerror(errno));
  }
}
