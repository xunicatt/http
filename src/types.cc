#include <types.h>
#include <format>
#include <string>
#include <sstream>

#ifndef HTTP_VERSION
  #define HTTP_VERSION "HTTP/1.1"
#endif

static void set_default_header_fields(http::Header&);
[[nodiscard]]
static std::string header_to_string(const http::Header&);

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

std::string to_string(const StatusCode& code) {
	switch (code) {
  	case StatusCode::Continue: return "Continue";
  	case StatusCode::SwitchingProtocols: return "Switching Protocols";
  	case StatusCode::Processing: return "Processing";
  	case StatusCode::EarlyHints: return "Early Hints";

  	case StatusCode::Ok: return "OK";
  	case StatusCode::Created: return "Created";
  	case StatusCode::Accepted: return "Accepted";
  	case StatusCode::NonAuthoritativeInformation: return "Non-Authoritative Information";
  	case StatusCode::NoContent: return "No Content";
  	case StatusCode::ResetContent: return "Reset Content";
  	case StatusCode::PartialContent: return "Partial Content";
  	case StatusCode::MultiStatus: return "Multi-Status";
  	case StatusCode::AlreadyReported: return "Already Reported";
  	case StatusCode::IMUsed: return "IM Used";

  	case StatusCode::MultipleChoices: return "Multiple Choices";
  	case StatusCode::MovedPermanently: return "Moved Permanently";
  	case StatusCode::Found: return "Found";
  	case StatusCode::SeeOther: return "See Other";
  	case StatusCode::NotModified: return "Not Modified";
  	case StatusCode::UseProxy: return "Use Proxy";
  	case StatusCode::TemporaryRedirect: return "Temporary Redirect";
  	case StatusCode::PermanentRedirect: return "Permanent Redirect";

  	case StatusCode::BadRequest: return "Bad Request";
  	case StatusCode::Unauthorized: return "Unauthorized";
  	case StatusCode::PaymentRequired: return "Payment Required";
  	case StatusCode::Forbidden: return "Forbidden";
  	case StatusCode::NotFound: return "Not Found";
  	case StatusCode::MethodNotAllowed: return "Method Not Allowed";
  	case StatusCode::NotAcceptable: return "Not Acceptable";
  	case StatusCode::ProxyAuthenticationRequired: return "Proxy Authentication Required";
  	case StatusCode::RequestTimeout: return "Request Timeout";
  	case StatusCode::Conflict: return "Conflict";
  	case StatusCode::Gone: return "Gone";
  	case StatusCode::LengthRequired: return "Length Required";
  	case StatusCode::PreconditionFailed: return "Precondition Failed";
  	case StatusCode::ContentTooLarge: return "Content Too Large";
  	case StatusCode::URITooLong: return "URI Too Long";
  	case StatusCode::UnsupportedMediaType: return "Unsupported Media Type";
  	case StatusCode::RangeNotSatisfiable: return "Range Not Satisfiable";
  	case StatusCode::ExpectationFailed: return "Expectation Failed";
  	case StatusCode::ImATeapot: return "I'm a teapot";
  	case StatusCode::MisdirectedRequest: return "Misdirected Request";
  	case StatusCode::UnprocessableContent: return "Unprocessable Content";
  	case StatusCode::Locked: return "Locked";
  	case StatusCode::FailedDependency: return "Failed Dependency";
  	case StatusCode::TooEarly: return "Too Early";
  	case StatusCode::UpgradeRequired: return "Upgrade Required";
  	case StatusCode::PreconditionRequired: return "Precondition Required";
  	case StatusCode::TooManyRequests: return "Too Many Requests";
  	case StatusCode::RequestHeaderFieldsTooLarge: return "Request Header Fields Too Large";
  	case StatusCode::UnavailableForLegalReasons: return "Unavailable For Legal Reasons";

  	case StatusCode::InternalServerError: return "Internal Server Error";
  	case StatusCode::NotImplemented: return "Not Implemented";
  	case StatusCode::BadGateway: return "Bad Gateway";
  	case StatusCode::ServiceUnavailable: return "Service Unavailable";
  	case StatusCode::GatewayTimeout: return "Gateway Timeout";
  	case StatusCode::HTTPVersionNotSupported: return "HTTP Version Not Supported";
  	case StatusCode::VariantAlsoNegotiates: return "Variant Also Negotiates";
  	case StatusCode::InsufficientStorage: return "Insufficient Storage";
  	case StatusCode::LoopDetected: return "Loop Detected";
  	case StatusCode::NotExtended: return "Not Extended";
  	case StatusCode::NetworkAuthenticationRequired: return "Network Authentication Required";

  	default: return std::string();
	}
}

Response::Response(const std::string& body)
: m_body(body), m_code(StatusCode::Ok) {
  set_default_header_fields(m_header);
  append_header("Content-Length", std::to_string(body.size()));
}

Response::Response(const StatusCode& code)
: m_body(""), m_code(code) {
  set_default_header_fields(m_header);
  append_header("Content-Length", std::to_string(0));
}

Response::Response(const std::string& body, const StatusCode& code)
: m_body(body), m_code(code) {
  set_default_header_fields(m_header);
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
    header_to_string(m_header),
    m_body
  );
}

std::vector<std::string> Request::segments() const {
  std::vector<std::string> result;
  std::stringstream ss(url);
  std::string token;

  while (std::getline(ss, token, '/')) {
    if (!token.empty()) {
      result.push_back(token);
  }
  }
  return result;
}
}

void set_default_header_fields(http::Header& header) {
  header.insert({"Connection", "Close"});
}

std::string header_to_string(const http::Header& header) {
  std::string res;
  for (const auto& [k, v]: header)
    res += std::format("{}: {}\n", k, v);
  return res;
}
