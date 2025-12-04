#include <router.h>
#include <logger.h>
#include <status.h>
#include <types.h>
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <format>
#include <optional>
#include <regex>
#include <unordered_map>
#include <sstream>
#include <string>
#include <sys/poll.h>
#include <sys/types.h>
#include <utility>
#include <unistd.h>

#ifndef CLIENT_POLLING_TIMEOUT
  #define CLIENT_POLLING_TIMEOUT 50
#endif

#ifndef CLIENT_READ_BUFFER_SIZE
  #define CLIENT_READ_BUFFER_SIZE 512
#endif

using ParsedURL = std::pair<std::string, std::unordered_map<std::string, std::string>>;

/* reads one line from client fd at a time */
[[nodiscard]]
static std::string readline(const int&);
/* parses the http request */
[[nodiscard]]
static std::optional<http::Request> parse(const int&);
/* parses only the http url */
[[nodiscard]]
static ParsedURL parse_url(const std::string&);
/* parses single header line */
[[nodiscard]]
static std::pair<std::string, std::string> parse_header_line(const std::string&);
/* parses entire http body */
[[nodiscard]]
static std::string parse_body(const int&, const std::optional<size_t>&);

static const std::unordered_map<std::string, http::Method> methods = {
  { "GET", http::Method::Get },
  { "POST", http::Method::Post },
  { "PUT", http::Method::Put },
  { "DELETE", http::Method::Delete },
};

namespace http {
void Router::add(const std::string& url, const Method& method, const RouteFunc& func) {
  if (!m_static_routes.contains(url)) {
    m_static_routes[url] = {{ method, func }};
    return;
  }

  auto& method_table = m_static_routes.at(url);
  method_table[method] = func;
}

void Router::add_regex(const std::string& url, const Method& method, const RouteFunc& func) {
  const auto& res = std::find_if(
    m_regex_routes.begin(),
    m_regex_routes.end(),
    [&url](const std::pair<RegexWrapper, RouteMethodTable>& item) {
      return item.first.string == url;
    }
  );

  if (res == m_regex_routes.end()) {
    m_regex_routes.emplace_back(
      RegexWrapper{ std::regex{url}, url },
      RouteMethodTable{{ method, func }}
    );
    return;
  }

  res->second[method] = func;
}

std::string Router::handle(const int& fd) const {
  const std::optional<Request> req_opt = parse(fd);
  if (!req_opt) {
    return Response(StatusCode::BadRequest).to_string();
  }

  const Request& req = *req_opt;
  info(std::format(
    "{} on {}",
    to_string(req.method),
    req.url
  ));

  if (m_static_routes.contains(req.url)) {
    const auto& method_table = m_static_routes.at(req.url);
    if (method_table.contains(req.method)) {
      const auto& func = method_table.at(req.method);
      return func(req).to_string();
    }
  }

  const auto& res = std::find_if(
    m_regex_routes.begin(),
    m_regex_routes.end(),
    [req](const std::pair<RegexWrapper, RouteMethodTable>& item) {
      if (std::regex_match(req.url, item.first.regex)) {
        return item.second.contains(req.method);
      }

      return false;
    }
  );

  if (res == m_regex_routes.end()) {
    warning(std::format(
      "url '{}' with method '{}' not found",
      req.url,
      to_string(req.method)
    ));
    return Response(StatusCode::NotFound).to_string();
  }

  const auto& method_table = res->second;
  const auto& func = method_table.at(req.method);
  return func(req).to_string();
}
}

std::optional<http::Request> parse(const int& fd) {
  /* TODO: error handling for length = 0 or bad request */
  http::debug("parsing request line");
  const std::string request_line = readline(fd);
  std::istringstream requst_line_stream(request_line);
  std::string method, raw_url, version;
  requst_line_stream >> method >> raw_url >> version;

  const auto &[url, params] = parse_url(raw_url);

  http::debug("parsing headers");
  http::Header header;
  while (true) {
    std::string header_line = readline(fd);
    if (header_line == "\r" || header_line.empty()) {
      break;
    }

    const auto& res = parse_header_line(header_line);
    if (res.first.length() != 0) {
      header.insert(res);
    }
  }

  if (!methods.contains(method)) {
    return std::nullopt;
  }

  return http::Request{
    .method = methods.at(method),
    .url = url,
    .params = params,
    .header = header,
    .body = parse_body(
      fd,
      header.contains("Content-Length") ?
        std::optional<size_t>(std::stol(header.at("Content-Length"))) :
        std::nullopt
    ) ,
  };
}

ParsedURL parse_url(const std::string& url) {
  http::debug("parsing url");
  const size_t pos = url.find("?");
  if (pos == std::string::npos) {
    return {url, {}};
  }

  const std::string str = url.substr(pos + 1);
  std::unordered_map<std::string, std::string> params;
  std::string key, value;
  bool setval = false;

  http::debug("parsing params");
  for (size_t i = 0; i < str.length(); i++) {
    switch (str[i]) {
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
        if (setval) {
          value += str[i];
        } else {
          key += str[i];
        }
      }
    }

    if (i == str.length() - 1) {
      params[key] = value;
    }
  }

  return {
    url.substr(0, pos),
    params
  };
}

std::pair<std::string, std::string> parse_header_line(const std::string& line) {
  const auto trim = [](std::string s) {
      s.erase(
        s.begin(),
        std::find_if(
          s.begin(),
          s.end(),
          [](unsigned char ch) {
            return !std::isspace(ch);
          }
        )
      );

      s.erase(
        std::find_if(
          s.rbegin(),
          s.rend(),
          [](unsigned char ch) {
            return !std::isspace(ch);
          }
        ).base(),
        s.end()
    );

    return s;
  };

  const size_t pos = line.find(":");
  if (pos == std::string::npos) {
    return {};
  }

  return { trim(line.substr(0, pos)), trim(line.substr(pos + 1)) };
}

std::string parse_body(const int& fd, const std::optional<size_t>& vlen) {
  http::debug("parsing body");
  if (vlen) {
    http::debug(std::format("got body length: {}", *vlen));
    size_t len = *vlen;
    char* buffer = new char[len + 1];

    len = read(fd, buffer, len);

    buffer[len] = '\0';
    std::string body{ buffer };
    delete[] buffer;

    return body;
  }

  thread_local static std::array<char, CLIENT_READ_BUFFER_SIZE> buffer;
  std::string body;

  pollfd fds[1] = {
    (pollfd){
      .fd = fd,
      .events = POLLIN,
      .revents = {}
    },
  };

  while(true) {
    http::debug("waiting for read using poll()");
    if (poll(fds, 1, CLIENT_POLLING_TIMEOUT) <= 0) {
      break;
    }

    if (fds[0].revents & POLLIN) {
      ssize_t len = 0;
      if (len = read(fd, buffer.data(), buffer.size() - 1); len <= 0) {
        break;
      }
      buffer[len] = '\0';
      body.append(buffer.data(), len);
    }
  }

  return body;
}

std::string readline(const int& fd) {
  std::string line; char ch;
  while (read(fd, &ch, 1) > 0) {
    if(ch == '\n') {
      break;
    }
    line += ch;
  }
  return line;
};
