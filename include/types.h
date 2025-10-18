#ifndef __HTTP_TYPES_H__
#define __HTTP_TYPES_H__

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <regex>
#include <variant>
#include <cstdint>

#include "status.h"
#include "methods.h"

namespace http {
/* Struct to handle server close interrupt */
struct SignalHandler {
  std::mutex mut;
  bool       close_intp;
};

using Header = std::unordered_map<std::string, std::string>;

struct RegexWrapper {
  std::regex  regex; /* regex */
  std::string string; /* regex string */
};

struct Request {
  Method      method;
  std::string url;
  std::unordered_map<std::string, std::string> params;
  Header      header;
  std::string body;

  /*
  * @brief Get array of segments of URL, example: if the url is /foo/bar/baz the array is [foo, bar, baz].
  * @return Array of URL segments.
  */
  [[nodiscard]]
  std::vector<std::string> segments() const;
};

struct Response {
public:
  /*
  * @brief Create http reponse with http::StatusOk and the given message.
  * @param body Body of the http response.
  */
  Response(const std::string& body);

  /*
  * @brief Create http reponse with http statuscode.
  * @param status_code Status code of the http response.
  */
  Response(const StatusCode& status_code);

  /*
  * @brief Create http reponse with http response body & http statuscode.
  * @param body Body of the http response.
  * @param status_code Status code of the http response.
  */
  Response(const std::string&,const StatusCode&);

  /*
  * @brief Append http options to the headers of the response.
  * @param id Name of the key.
  * @param value Value of the field.
  */
  void append_header(const std::string& id, const std::string& value);

  /*
  * @brief Set the message body of the response.
  * @param body Body of the http response.
  */
  void set_body(const std::string& body);

  /*
  * @brief Set the statuscode of the response.
  * @param status_code Status code of the http response.
  */
  void set_code(const StatusCode& status_code);

  /*
  * @brief Convert http response to string.
  * @return String fromat of the http reponse.
  */
  [[nodiscard]]
  std::string to_string() const;

private:
  std::string body;
  StatusCode  code;
  Header      header;
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

using Array = std::vector<Node>;
using Object = std::unordered_map<std::string, Node>;
using Data = std::variant<
  int64_t,
  double,
  bool,
  std::string,
  Array,
  Object
>;

class Node {
public:
  // to keep backwards compatibility
  // Node constructor with int is left out
  // as an option
  Node(const int&);
  Node(const int64_t&);
  Node(const double&);
  Node(const bool&);
  Node(const char*);
  Node(const std::string&);
  Node(const Array&);
  Node(const Object&);

  /*
  * @brief Get the Type of the Node.
  * @return NodeType.
  */
  [[nodiscard]]
  const NodeType& type() const;

  /*
  * @brief Get the Immutable Internal Data vairant of the Node.
  * @return Data.
  */
  [[nodiscard]]
  const Data& get() const;

  /*
  * @brief Get the mutable Internal Data vairant of the Node.
  * @return Data.
  */
  [[nodiscard]]
  Data& get();

  /*
  * @brief Get the mutable Data of the requested type from the Node.
  * @return T.
  */
  template <typename T>
  [[nodiscard]]
  T& get();

  /*
  * @brief Get the immutable Data of the requested type from the Node.
  * @return T.
  */
  template <typename T>
  [[nodiscard]]
  const T& get() const;

  /*
  * @brief Converts a node to equivalent json string.
  * @return Json String format of the node.
  */
  [[nodiscard]]
  std::string to_string() const;

private:
  Data     data;
  NodeType _type;
};
} // json namespace end
} // http namespace end

#endif
