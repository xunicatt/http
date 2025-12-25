#include <lime/lime.h>

namespace lime {
  namespace http {
    std::string to_string(const Method& m) {
      switch (m) {
        case Method::Get:    return "GET";
        case Method::Post:   return "POST";
        case Method::Put:    return "PUT";
        case Method::Delete: return "DELETE";
        default:             return {};
      }
    }
  } // http
} // lime
