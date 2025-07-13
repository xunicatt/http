#ifndef __HTTP_METHODS_H__
#define __HTTP_METHODS_H__

#include <string>

namespace http {
enum class Method {
  Get,
  Post,
  Put,
  Delete,
};

[[nodiscard]]
std::string to_string(const Method&);
}
#endif
