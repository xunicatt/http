#ifndef __HTTP_METHODS_H__
#define __HTTP_METHODS_H__

#include <string>

namespace http {
enum Method {
  GET,
  POST,
  PUT,
  DELETE,
};

std::string to_string(const Method&);
}
#endif
