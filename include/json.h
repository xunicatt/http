#ifndef __JSON_H__
#define __JSON_H__

#include <expected>

#include "types.h"

namespace http {
namespace json {
template <typename T>
T& Node::get() {
  return std::get<T>(data);
}

template <typename T>
const T& Node::get() const {
  return std::get<T>(data);
}

[[nodiscard]]
std::string encode(const Node&);
[[nodiscard]]
std::expected<Node, std::string> decode(const std::string&);
} // json namespace end
} // http namespace end

#endif
