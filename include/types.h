#ifndef __HTTP_TYPES_H__
#define __HTTP_TYPES_H__

#include <mutex>
#include <string>
#include <map>
#include <vector>

#include "status.h"
#include "methods.h"

namespace http {
struct SignalHandler {
  std::mutex mut;
  bool close_intp;
};

using Header = std::map<std::string, std::string>;

struct Request {
  Method method;
  std::string url;
  std::map<std::string, std::string> params;
  Header header;
  std::string body;

  [[nodiscard]]
  std::vector<std::string> segments() const;
};

struct Response {
public:
  Response(const std::string&);
  Response(const HttpStatusCode&);
  Response(const std::string&,const HttpStatusCode&);

  void append_header(const std::string&,const std::string&);
  void set_body(const std::string&);
  void set_code(const HttpStatusCode&);

  [[nodiscard]]
  std::string to_string() const;

private:
  std::string body;
  HttpStatusCode code;
  Header header;
};
}

#endif
