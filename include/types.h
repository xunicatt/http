#ifndef __HTTP_TYPES_H__
#define __HTTP_TYPES_H__

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <regex>

#include "status.h"
#include "methods.h"

namespace http {
/* Struct to handle server close interrupt */
struct SignalHandler {
  std::mutex mut;
  bool       close_intp;
};

using Header = std::unordered_map<std::string, std::string>;

struct RegexWrapper {
  std::regex  regex; /* regex */
  std::string string; /* regex string */
};

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

struct Response {
public:
  /*
  * @brief Create http reponse with http::StatusOk and the given message.
  * @param body Body of the http response.
  */
  Response(const std::string& body);

  /*
  * @brief Create http reponse with http statuscode.
  * @param status_code Status code of the http response.
  */
  Response(const StatusCode& status_code);

  /*
  * @brief Create http reponse with http response body & http statuscode.
  * @param body Body of the http response.
  * @param status_code Status code of the http response.
  */
  Response(const std::string&,const StatusCode&);

  /*
  * @brief Append http options to the headers of the response.
  * @param id Name of the key.
  * @param value Value of the field.
  */
  void append_header(const std::string& id, const std::string& value);

  /*
  * @brief Set the message body of the response.
  * @param body Body of the http response.
  */
  void set_body(const std::string& body);

  /*
  * @brief Set the statuscode of the response.
  * @param status_code Status code of the http response.
  */
  void set_code(const StatusCode& status_code);

  /*
  * @brief Convert http response to string.
  * @return String fromat of the http reponse.
  */
  [[nodiscard]]
  std::string to_string() const;

private:
  std::string m_body;
  StatusCode  m_code;
  Header      m_header;
};
} // http namespace end

#endif
