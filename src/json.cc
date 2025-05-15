#ifdef HTTP_EXPERIMENTAL_MODULES
#include <json.h>
#include <format>
#include <ranges>
#include <print>

static std::string json_value_to_string(const json::JsonValue&);

namespace json {
std::string to_string(const JsonValueType& t) {
  switch(t) {
    case JsonValueType::Int: return "int";
    case JsonValueType::Float: return "float";
    case JsonValueType::Bool: return "bool";
    case JsonValueType::String: return "string";
    case JsonValueType::Array: return "array";
    case JsonValueType::Object: return "object";
    default: return {};
  }
}

Value::Value(const int& v)
: value(v), _type(JsonValueType::Int) {}

Value::Value(const int64_t& v)
: value(v), _type(JsonValueType::Int) {}

Value::Value(const double& v)
: value(v), _type(JsonValueType::Float) {}

Value::Value(const bool& v)
: value(v), _type(JsonValueType::Bool) {}

Value::Value(const std::string& v)
: value(v), _type(JsonValueType::String) {}

Value::Value(const char* v)
: value(v), _type(JsonValueType::String) {}

Value::Value(const Array& v)
: value(v), _type(JsonValueType::Array) {}

Value::Value(const Object& v)
: value(v), _type(JsonValueType::Object) {}

const JsonValueType& Value::type() const { return _type; }
const JsonValue& Value::get() const { return value; }
JsonValue& Value::get() { return value; }

std::string to_string(const Value& value) {
  return json_value_to_string(value.get());
}

std::string encode(const Value& value) {
  return to_string(value);
}
}

static std::string json_value_to_string(
  const json::JsonValue& val
) {
  switch((json::JsonValueType)val.index()) {
    case json::JsonValueType::Int:
      return std::to_string(std::get<int64_t>(val));

    case json::JsonValueType::Float:
      return std::to_string(std::get<double>(val));

    case json::JsonValueType::Bool:
      return std::format("{}", std::get<bool>(val)?"true":"false");

    case json::JsonValueType::String:
      return std::format("\"{}\"", std::get<std::string>(val));

    case json::JsonValueType::Array: {
      std::string res = "[";
      auto const& arr = std::get<json::Array>(val);
      for(const auto& [i, item]: arr | std::views::enumerate) {
        res += json_value_to_string(item.get());
        if(static_cast<size_t>(i) < arr.size() - 1) res += ", ";
      }
      res += "]";
      return res;
    }

    case json::JsonValueType::Object: {
      std::string res = "{";
      const auto& obj = std::get<json::Object>(val);
      size_t i = 0;
      for(auto const& [k, v]: std::get<json::Object>(val)) {
        res += std::format("\"{}\": {}", k, json_value_to_string(v.get()));
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
