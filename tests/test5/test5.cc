#include <cstdint>
#include <lime/lime.h>
#include <cstring>

namespace http = lime::http;
namespace json = lime::json;

static constexpr int generate_otp() {
  // INFO: to validate the output using test
  // the number has to be a static
  return 456123;
}

static http::Response get_otp(const http::Request& req)  {
  if(!req.params.contains("name")) {
    return http::StatusCode::BadRequest;
  }

  json::Node root = json::Object {
    { "name", req.params.at("name") },
    { "otp", generate_otp() }
  };

  return json::encode(root);
}

static http::Response validate_otp(const http::Request& req) {
  const auto res = json::decode(req.body);
  if(!res.has_value()) {
    return http::Response(
      res.error(),
      http::StatusCode::BadRequest
    );
  }

  // Get the root json object
  const auto& root = res.value().get<json::Object>();

  if(!root.contains("name") || !root.contains("otp")) {
    return http::StatusCode::BadRequest;
  }

  const auto& name_node = root.at("name");
  const auto& otp_node = root.at("otp");

  if(name_node.type() != json::NodeType::String || otp_node.type() != json::NodeType::Int) {
    return http::StatusCode::BadRequest;
  }

  const auto& otp = otp_node.get<int64_t>();
  if(otp != generate_otp()) {
    return json::encode(json::Object{{"msg", "invalid otp"}});
  }

  return json::encode(json::Object{{"msg", "success"}});
}

int main() {
  http::Router router;
  router.add("/get-otp", http::Method::Get, get_otp);
  router.add("/validate-otp", http::Method::Post, validate_otp);

  http::Server server(router);

  if(server.port(8080).run() < 0) {
    std::perror(std::strerror(errno));
    std::exit(EXIT_FAILURE);
  }

  return 0;
}
