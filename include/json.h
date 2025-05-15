#ifndef __JSON_H__
#define __JSON_H__

#ifndef HTTP_EXPERIMENTAL_MODULES
  #error Enable "HTTP_EXPERIMENTAL_MODULES" with -D flag to use this experimental "json" module
#endif

#include <variant>
#include <cstdint>
#include <string>
#include <vector>
#include <map>

namespace json {
class Value;
enum class JsonValueType {
  Int,
  Float,
  Bool,
  String,
  Array,
  Object,
};

[[nodiscard]]
std::string to_string(const JsonValueType&);
[[nodiscard]]
std::string to_string(const Value&);

using Array = std::vector<Value>;
using Object = std::map<std::string, Value>;
using JsonValue = std::variant<
  int64_t,
  double,
  bool,
  std::string,
  Array,
  Object
>;

class Value {
public:
  Value(const int&);
  Value(const int64_t&);
  Value(const double&);
  Value(const bool&);
  Value(const std::string&);
  Value(const char*);
  Value(const Array&);
  Value(const Object&);

  [[nodiscard]]
  const JsonValueType& type() const;
  [[nodiscard]]
  const JsonValue& get() const;
  [[nodiscard]]
  JsonValue& get();
  template<typename T>
  [[nodiscard]]
  T get() const;

private:
  JsonValue value;
  JsonValueType _type;
};

template<typename T>
T Value::get() const {
  return std::get<T>(value);
}

// NOT IMPLEMENTED YET
/*
[[nodiscard]]
Value decode(const std::string&);
*/
[[nodiscard]]
std::string encode(const Value&);
}



#endif
