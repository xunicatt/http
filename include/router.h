#ifndef __HTTP_ROUTER_H__
#define __HTTP_ROUTER_H__

#include <functional>
#include <map>
#include <string>

#include "types.h"

namespace http {
using RouteFunc = std::function<Response(const Request&)>;

struct RouteHandler {
  Method method;
  RouteFunc func;
};

class Router {
public:
  void add(const std::string&, const Method&, const RouteFunc&);
  std::string handle(const int&) const;

private:
  std::map<std::string, RouteHandler> routes;
};
}

#endif
