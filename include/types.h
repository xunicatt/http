#ifndef __HTTP_TYPES_H__
#define __HTTP_TYPES_H__

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <regex>
#include <variant>

#include "status.h"
#include "methods.h"

namespace http {
struct SignalHandler {
  std::mutex mut;
  bool close_intp;
};

using Header = std::unordered_map<std::string, std::string>;

struct RegexWrapper {
  std::regex regex;
  std::string string;
};

struct Request {
  Method method;
  std::string url;
  std::unordered_map<std::string, std::string> params;
  Header header;
  std::string body;

  [[nodiscard]]
  std::vector<std::string> segments() const;
};

struct Response {
public:
  Response(const std::string&);
  Response(const StatusCode&);
  Response(const std::string&,const StatusCode&);

  void append_header(const std::string&,const std::string&);
  void set_body(const std::string&);
  void set_code(const StatusCode&);

  [[nodiscard]]
  std::string to_string() const;

private:
  std::string body;
  StatusCode code;
  Header header;
};

namespace json {
class Node;
enum class NodeType {
  Int,
  Float,
  Bool,
  String,
  Array,
  Object,
};

[[nodiscard]]
std::string to_string(const NodeType&);

// TODO: integer should be handled using int64_t or uint64_t eventually
using Array = std::vector<Node>;
using Object = std::unordered_map<std::string, Node>;
using Data = std::variant<
  int,
  double,
  bool,
  std::string,
  Array,
  Object
>;

class Node {
public:
  Node(const int&);
  Node(const double&);
  Node(const bool&);
  Node(const char*);
  Node(const std::string&);
  Node(const Array&);
  Node(const Object&);

  [[nodiscard]]
  const NodeType& type() const;
  [[nodiscard]]
  const Data& get() const;
  [[nodiscard]]
  Data& get();
  template <typename T>
  [[nodiscard]]
  T& get();
  template <typename T>
  [[nodiscard]]
  const T& get() const;
  [[nodiscard]]
  std::string to_string() const;

private:
  Data data;
  NodeType _type;
};
} // json namespace end
} // http namespace end

#endif
