#ifndef __JSON_H__
#define __JSON_H__

#ifdef HTTP_EXPERIMENTAL_MODULES

#include <expected>

#include "types.h"

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
}

#endif
#endif
