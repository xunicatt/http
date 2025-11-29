#ifndef __JSON_H__
#define __JSON_H__

#include <expected>

#include "types.h"

namespace http {
namespace json {
template <typename T>
inline T& Node::get() {
  return std::get<T>(data);
}

template <typename T>
inline const T& Node::get() const {
  return std::get<T>(data);
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
