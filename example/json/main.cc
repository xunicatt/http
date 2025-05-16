#include <print>
#include <cstring>
#include <http.h>

http::Response user(const http::Request& req) {
  const auto bad_request = http::Response(http::HttpStatusCode::BadGateway);
  if(!req.header.contains("Content-Type")) {
    return bad_request;
  }

  if(req.header.at("Content-Type") != "application/json") {
    return bad_request;
  }

  const auto& decoded_json = json::decode(req.body);
  if(!decoded_json.has_value()) {
    return http::Response(
      decoded_json.error(),
      http::HttpStatusCode::BadRequest
    );
  }

  const auto& data = decoded_json.value();
  const auto& obj = data.get<json::Object>();
  if(!obj.contains("name")) {
    return bad_request;
  }

  if(obj.at("name").type() != json::NodeType::String) {
    return bad_request;
  }

  const auto& name = obj.at("name").get<std::string>();

  json::Node node = json::Object{
    {"user", name},
    {"message", "welcome"}
  };

  http::Response resp(json::encode(node));
  resp.append_header("Content-Type", "application/json");
  return resp;
}

int main() {
  http::Router router;
  router.add("/user", http::GET, user);

  http::Server server(router);
  if(server.run() < 0) {
    std::println(stderr, "failed to run server: {}", strerror(errno));
    exit(EXIT_FAILURE);
  }
}
