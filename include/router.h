#ifndef __HTTP_ROUTER_H__
#define __HTTP_ROUTER_H__

#include <functional>
#include <unordered_map>
#include <string>

#include "types.h"

namespace http {
using RouteFunc = std::function<Response(const Request&)>;

class Router {
public:
  void add(const std::string&, const Method&, const RouteFunc&);
  void add_regex(const std::string&, const Method&, const RouteFunc&);
  [[nodiscard]]
  std::string handle(const int&) const;

private:
  using RouteMethodTable = std::unordered_map<http::Method, RouteFunc>;
  std::unordered_map<std::string, RouteMethodTable> static_routes;
  std::vector<
    std::pair<
      RegexWrapper,
      RouteMethodTable
    >
  > regex_routes;
};
}

#endif
