#include <router.h>
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <format>
#include <optional>
#include <print>
#include <regex>
#include <sstream>
#include <string>
#include <utility>
#include <unistd.h>

#include <logger.h>
#include <status.h>
#include <types.h>

using ParsedURL = std::pair<std::string, std::map<std::string, std::string>>;

static ParsedURL parse_url(const std::string&);
static std::pair<std::string, std::string> parse_header_line(const std::string&);
static std::optional<http::Request> parse(const int&);
static std::string readline(const int&);

static const std::map<std::string, http::Method> methods = {
  {"GET", http::Method::GET},
  {"POST", http::Method::POST},
  {"PUT", http::Method::PUT},
  {"DELETE", http::Method::DELETE},
};

namespace http {
void Router::add(const std::string& url, const Method& method, const RouteFunc& func) {
  routes[url] = RouteHandler{.method = method, .func = func};
}

std::string Router::handle(const int& fd) const {
  const std::optional<Request> req_opt = parse(fd);
  if(!req_opt.has_value()) {
    return Response(HttpStatusCode::BadRequest).to_string();
  }

  const Request& req = req_opt.value();
  info(std::format(
    "{} on {}",
    to_string(req.method),
    req.url
  ));

  /* I know its inefficient to itterate over all routes */
  RouteHandler rh;
  const auto& res = std::find_if(
    routes.begin(),
    routes.end(),
    [req](const std::pair<std::string, RouteHandler>& kv) {
      const std::regex pattern(kv.first);
      return std::regex_match(req.url, pattern);
    }
  );

  if(res == routes.end()) {
    warning(std::format("url '{}' not found", req.url));
    return Response(HttpStatusCode::NotFound).to_string();
  }

  rh = res->second;

  if(rh.method != req.method) {
    warning(std::format(
      "expected method '{}' on url '{}' but got '{}'",
      to_string(rh.method),
      req.url,
      to_string(req.method)
    ));
    return Response(HttpStatusCode::MethodNotAllowed).to_string();
  }

  return rh.func(req).to_string();
}
}

std::optional<http::Request> parse(const int& fd) {
  /* [TODO] error handling for length = 0 or bad request */
  http::debug("parsing request line");
  const std::string request_line = readline(fd);
  std::istringstream requst_line_stream(request_line);
  std::string method, raw_url, version;
  requst_line_stream >> method >> raw_url >> version;

  const auto &[url, params] = parse_url(raw_url);

  http::debug("parsing headers");
  http::Header header;
  while(true) {
    std::string header_line = readline(fd);
    if(header_line == "\r" || header_line.empty()) break;

    const auto& res = parse_header_line(header_line);
    if(res.first.length() != 0) {
      header.insert(res);
    }
  }

  if(!methods.contains(method)) {
    return std::nullopt;
  }

  return http::Request{
    .method = methods.at(method),
    .url = url,
    .params = params,
    .header = header,
  };
}

ParsedURL parse_url(const std::string& url) {
  http::debug("parsing url");
  const size_t pos = url.find("?");
  if(pos == std::string::npos) {
    return {url, {}};
  }

  const std::string str = url.substr(pos + 1);
  std::map<std::string, std::string> params;
  std::string key, value;
  bool setval = false;

  http::debug("parsing params");
  for(size_t i = 0; i < str.length(); i++) {
    switch(str[i]) {
      case '&': {
        params[key] = value;
        key.clear(); value.clear();
        setval = false;
        break;
      }

      case '=': {
        setval = true;
        break;
      }

      default: {
        if(setval) value += str[i];
        else key += str[i];
      }
    }

    if(i == str.length() - 1)
      params[key] = value;
  }

  return {
    url.substr(0, pos),
    params
  };
}

std::pair<std::string, std::string> parse_header_line(const std::string& line) {
  const size_t pos = line.find(":");
  if(pos == std::string::npos) {
    return {};
  }

  return {line.substr(0, pos), line.substr(pos + 1)};
}

std::string readline(const int& fd) {
  std::string line; char ch;
  while(read(fd, &ch, 1) > 0) {
    if(ch == '\n') break;
    line += ch;
  }
  return line;
};
