#include <json.h>

#ifdef HTTP_EXPERIMENTAL_MODULES

#include <parser.h>
#include <format>
#include <ranges>

[[nodiscard]]
static std::string data_to_string(const json::Data&);

namespace json {
Node::Node(const int& data)
: data(data), _type(NodeType::Int) {}

Node::Node(const double& data)
: data(data), _type(NodeType::Float) {}

Node::Node(const bool& data)
: data(data), _type(NodeType::Bool) {}

Node::Node(const char* data)
: data(data), _type(NodeType::String) {}

Node::Node(const std::string& data)
: data(data), _type(NodeType::String) {}

Node::Node(const Array& data)
: data(data), _type(NodeType::Array) {}

Node::Node(const Object& data)
: data(data), _type(NodeType::Object){}

const NodeType& Node::type() const {
  return _type;
}

const Data& Node::get() const {
  return data;
}

Data& Node::get() {
  return data;
}

std::string Node::to_string() const {
  return data_to_string(data);
}

std::string encode(const Node& node) {
  return node.to_string();
}

std::expected<Node, std::string> decode(const std::string& data) {
  Scanner scanner(data);
  Parser parser(scanner);
  return parser.parse();
}
}

std::string data_to_string(const json::Data& data) {
  switch(static_cast<json::NodeType>(data.index())) {
    case json::NodeType::Int:
      return std::to_string(std::get<int>(data));

    case json::NodeType::Float:
      return std::to_string(std::get<double>(data));

    case json::NodeType::Bool:
      return std::get<bool>(data) ? "true" : "false";

    case json::NodeType::String:
      return std::format("\"{}\"", std::get<std::string>(data));

    case json::NodeType::Array: {
      std::string res = "[";
      auto const& arr = std::get<json::Array>(data);
      for(const auto& [i, x]: arr | std::views::enumerate) {
        res += data_to_string(x.get());
        if(static_cast<size_t>(i) < arr.size() - 1) res += ", ";
      }
      res += "]";
      return res;
    }

    case json::NodeType::Object: {
      std::string res = "{"; size_t i = 0;
      const auto& obj = std::get<json::Object>(data);
      for(auto const& [k, v]: obj) {
        res += std::format("\"{}\": {}", k, data_to_string(v.get()));
        if(i < obj.size() - 1) res += ", ";
        i++;
      }
      res += "}";
      return res;
    }

    default:
      return {};
  }
}

#endif
