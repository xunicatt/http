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
  /*
  * @brief Adds new route to the router.
  * @param path Url path.
  * @param method Http Method
  * @param handler Http Request handler function
  */
  void add(const std::string& path, const Method& method, const RouteFunc& handler);

  /*
  * @brief Adds new regex based route to the router.
  * @param path Url path.
  * @param method Http Method
  * @param handler Http Request handler function
  */
  void add_regex(const std::string&, const Method&, const RouteFunc&);

  [[nodiscard]]
  std::string handle(const int&) const;

private:
  // TODO: simplify whatever fuck this is!
  using RouteMethodTable = std::unordered_map<http::Method, RouteFunc>;
  std::unordered_map<std::string, RouteMethodTable> m_static_routes;
  std::vector<
    std::pair<
      RegexWrapper,
      RouteMethodTable
    >
  > m_regex_routes;
};
}

#endif
