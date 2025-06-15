#include <http.h>
#include <cstring>

static constexpr int generate_otp() {
  // INFO> to validate the output using test
  // the number has to be a static
  return 456123;
}

static http::Response get_otp(const http::Request& req)  {
  namespace json = http::json;

  if(!req.params.contains("name")) {
    return http::HttpStatusCode::BadRequest;
  }
  
  json::Node root = json::Object {
    { "name", req.params.at("name") },
    { "otp", generate_otp() }
  };

  return json::encode(root);
}

static http::Response validate_otp(const http::Request& req) {
  namespace json = http::json;

  const auto res = json::decode(req.body);
  if(!res.has_value()) {
    return http::Response(
      res.error(),
      http::HttpStatusCode::BadRequest
    );
  }

  // Get the root json object
  const auto& root = res.value().get<json::Object>();

  if(!root.contains("name") || !root.contains("otp")) {
    return http::BadRequest;
  }

  const auto& name_node = root.at("name");
  const auto& otp_node = root.at("otp");

  if(name_node.type() != json::NodeType::String || otp_node.type() != json::NodeType::Int) {
    return http::BadRequest;
  }

  const auto& otp = otp_node.get<int>();
  if(otp != generate_otp()) {
    return json::encode(json::Object{{"msg", "invalid otp"}});
  }

  return json::encode(json::Object{{"msg", "success"}});
}

int main() {
  http::Router router;
  router.add("/get-otp", http::Method::GET, get_otp);
  router.add("/validate-otp", http::Method::POST, validate_otp);

  http::Server server(router);

  if(server.port(8080).run() < 0) {
    std::perror(std::strerror(errno));
    std::exit(EXIT_FAILURE);
  }

  return 0;
}
