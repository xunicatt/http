#include <print>
#include <lime.h>

namespace json = lime::json;

static void encoding();
static void decoding();

int main() {
  encoding();
  decoding();
}

void encoding() {
  // json encoding
  json::Node root_node = json::Object{
    // key - value
    { "name", "John" },
    { "id", 34 },
    { "backend-dev", true },
    { "languages", json::Array{
        "c",
        "cpp",
        "js",
        "go"
    }},
    { "projects", json::Array{
      json::Object{
        { "name", "chat-server-backend" },
        { "language", "go" }
      },
      json::Object{
        { "name", "compression-engine" },
        { "language", "cpp" }
      }
    }}
  };

  // insert an item later
  auto& root = root_node.get<json::Object>();
  root.insert({ "married", false });

  // get an sub object
  // and modify it too
  auto& languages = root.at("languages").get<json::Array>();
  languages.emplace_back("rust");

  const std::string result = json::encode(root);
  std::println("{}", result);
}

void decoding() {
  std::string data = R"({
    "user-name": "Bill",
    "active-since": "10-04-2021",
    "order-ids": [ 234, 456, 587, 599, 782 ],
    "location": {
      "office": "Denmark"
    }
  })";

  // decode returns std::expected which
  // contains either the error as std::string
  // or the decoded data
  auto result = json::decode(data);
  if(!result.has_value()) {
    // error is typically formated like: <row>:<col> <error-message>
    std::println(stderr, "decoding error: {}", result.error());
    return;
  }

  json::Node& root_node = result.value();
  // now to get an item or check its type
  // type() or get() methods can be used
  json::Object& root = root_node.get<json::Object>();

  // json::Object is nothing but a wrapper around std::unordered_map
  // all functions of std::unordered_map are available
  if(root.contains("user-name")) {
    auto& user_name_node = root.at("user-name");
    // to check the type use type() on any node
    if(user_name_node.type() != json::NodeType::String) {
      std::println("error: expected user-name to be a string");
      return;
    }

    // to_string converts the node to equivalent string
    // representation, here for string it is "<data>"
    std::println("node> username: {}", user_name_node.to_string());

    // or get the string directly using get
    auto& user_name = user_name_node.get<std::string>();
    std::println("raw> username: {}", user_name);

    // values in json can be modified by taking reference to it
    user_name = "Kyle";
  }

  if(root.contains("order-ids")) {
    auto& order_ids_node = root.at("order-ids");
    if(order_ids_node.type() != json::NodeType::Array) {
      std::println("error: expected order-ids to be an array");
      return;
    }

    auto& order_ids = order_ids_node.get<json::Array>();
    std::println("order-ids:");
    for(const auto& id: order_ids) {
      // each element is an array is a node too
      // to get the type use type() or for value use get<TYPE>()
      std::println("\t{}", id.to_string());
    }

    // adding a new order-id
    order_ids.emplace_back(1023);
  }

  std::println("modified json:\n{}", json::encode(root));
}
