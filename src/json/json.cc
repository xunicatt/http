#include <lime/lime.h>
#include <lime/json/parser.h>
#include <format>

namespace lime {
  namespace json {
    namespace data {
      [[nodiscard]]
      static std::string to_string(const json::Data& data) {
        switch (static_cast<json::NodeType>(data.index())) {
          case json::NodeType::Int:
          return std::to_string(std::get<int64_t>(data));

          case json::NodeType::Float:
          return std::to_string(std::get<double>(data));

          case json::NodeType::Bool:
          return std::get<bool>(data) ? "true" : "false";

          case json::NodeType::String:
          return std::format("\"{}\"", std::get<std::string>(data));

          case json::NodeType::Array: {
            std::string res { "[" }; size_t i { 0 };
            const auto& arr { std::get<json::Array>(data) };
            for (const auto& x: arr) {
              res += data::to_string(x.get());
              if (i < arr.size() - 1) {
                res += ", ";
              }
            }
            res += "]";
            return res;
          }

          case json::NodeType::Object: {
            std::string res { "{" }; size_t i { 0 };
            const auto& obj { std::get<json::Object>(data) };
            for (const auto& [k, v]: obj) {
              res += std::format("\"{}\": {}", k, data::to_string(v.get()));
              if (i < obj.size() - 1) {
                res += ", ";
              }
              i++;
            }
            res += "}";
            return res;
          }

          default:
          return {};
        }
      }
    }

    Node::Node(const int& data)
    : m_data(data), m_type(NodeType::Int) {}

    Node::Node(const int64_t& data)
    : m_data(data), m_type(NodeType::Int) {}

    Node::Node(const double& data)
    : m_data(data), m_type(NodeType::Float) {}

    Node::Node(const bool& data)
    : m_data(data), m_type(NodeType::Bool) {}

    Node::Node(const char* data)
    : m_data(data), m_type(NodeType::String) {}

    Node::Node(const std::string& data)
    : m_data(data), m_type(NodeType::String) {}

    Node::Node(const Array& data)
    : m_data(data), m_type(NodeType::Array) {}

    Node::Node(const Object& data)
    : m_data(data), m_type(NodeType::Object){}

    const NodeType& Node::type() const {
      return m_type;
    }

    const Data& Node::get() const {
      return m_data;
    }

    Data& Node::get() {
      return m_data;
    }

    std::string Node::to_string() const {
      return data::to_string(m_data);
    }

    std::string encode(const Node& node) {
      return node.to_string();
    }

    std::expected<Node, std::string> decode(const std::string& data) {
      Scanner scanner { data };
      Parser  parser { scanner };
      return parser.parse();
    }
  } // json
} // http
