#ifndef __JSON_H__
#define __JSON_H__

#include <cstdint>
#include <string>
#include <expected>
#include <vector>
#include <unordered_map>
#include <variant>

#include "types.h"

namespace http {
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
  Data     m_data;
  NodeType m_type;
};

template <typename T>
inline T& Node::get() {
  return std::get<T>(m_data);
}

template <typename T>
inline const T& Node::get() const {
  return std::get<T>(m_data);
}

/*
* @brief Encode the given json Node to equivalent json String.
* @return String.
*/
[[nodiscard]]
std::string encode(const Node&);

/*
* @brief Decode the given json string to equivalent json Node.
* @return <Node, String> Node the the Value, String is the Error.
*/
[[nodiscard]]
std::expected<Node, std::string> decode(const std::string&);
} // json namespace end
} // http namespace end

#endif
