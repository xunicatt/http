#include <format>
#include <lime/http/request.h>
#include <string>
#include <lime/lime.h>

#ifndef HTTP_VERSION
  #define HTTP_VERSION "HTTP/1.1"
#endif

namespace lime {
  namespace http {
    namespace header {
      static void set_defaults(http::Header& header) {
        header.insert({ "Connection", "Close" });
      }

      [[nodiscard]]
      static std::string to_string(const http::Header& header) {
        std::string res {};
        for (const auto& [k, v]: header)
          res += std::format("{}: {}\n", k, v);
        return res;
      }
    } // header

    Response::Response(const std::string& body)
    : m_body(body), m_code(StatusCode::Ok) {
      header::set_defaults(m_header);
      append_header("Content-Length", std::to_string(body.size()));
    }

    Response::Response(const StatusCode& code)
    : m_body(""), m_code(code) {
      header::set_defaults(m_header);
      append_header("Content-Length", std::to_string(0));
    }

    Response::Response(const std::string& body, const StatusCode& code)
    : m_body(body), m_code(code) {
      header::set_defaults(m_header);
      append_header("Content-Length", std::to_string(body.size()));
    }

    void Response::append_header(const std::string& key, const std::string& value) {
      m_header.insert({ key, value });
    }
    void Response::set_body(const std::string& vbody) {
      m_body = vbody;
      append_header("Content-Length", std::to_string(m_body.size()));
    }

    void Response::set_code(const StatusCode& vcode) {
      m_code = vcode;
    }

    std::string Response::to_string() const {
      return std::format(
        HTTP_VERSION" {} {}\n{}\r\n{}",
        static_cast<int>(m_code),
        http::to_string(m_code),
        header::to_string(m_header),
        m_body
      );
    }
  } // http
} // lime
