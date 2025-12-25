#ifndef LIME_HTTP_REQUEST_H
#define LIME_HTTP_REQUEST_H

#include <vector>
#include <unordered_map>

#include "methods.h"

namespace lime {
  namespace http {
    using Header = std::unordered_map<std::string, std::string>;

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
  } // http
} // lime

#endif // LIME_HTTP_REQUEST_H
