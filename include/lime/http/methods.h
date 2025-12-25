#ifndef LIME_HTTP_METHODS_H
#define LIME_HTTP_METHODS_H

#include <string>

namespace lime {
  namespace http {
    enum class Method {
      Get,
      Post,
      Put,
      Delete,
    };

    [[nodiscard]]
    std::string to_string(const Method&);
  } // http
} // lime

#endif // LIME_HTTP_METHODS_H
