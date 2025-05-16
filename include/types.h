#ifndef __HTTP_TYPES_H__
#define __HTTP_TYPES_H__

#include <mutex>
#include <string>
#include <map>
#include <vector>
#include <variant>

#include "status.h"
#include "methods.h"

namespace http {
struct SignalHandler {
  std::mutex mut;
  bool close_intp;
};

using Header = std::map<std::string, std::string>;

struct Request {
  Method method;
  std::string url;
  std::map<std::string, std::string> params;
  Header header;
  std::string body;

  [[nodiscard]]
  std::vector<std::string> segments() const;
};

struct Response {
public:
  Response(const std::string&);
  Response(const HttpStatusCode&);
  Response(const std::string&,const HttpStatusCode&);

  void append_header(const std::string&,const std::string&);
  void set_body(const std::string&);
  void set_code(const HttpStatusCode&);

  [[nodiscard]]
  std::string to_string() const;

private:
  std::string body;
  HttpStatusCode code;
  Header header;
};
}

#ifdef HTTP_EXPERIMENTAL_MODULES

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

using Array = std::vector<Node>;
using Object = std::map<std::string, Node>;
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
}
#endif

#endif
