#include <types.h>
#include <format>
#include <string>
#include <sstream>

static void set_default_header_fields(http::Header&);
static std::string header_to_string(const http::Header&);

namespace http {
std::string to_string(const Method& m) {
  switch(m) {
    case Method::GET: return "GET";
    case Method::POST: return "POST";
    case Method::PUT: return "PUT";
    case Method::DELETE: return "DELETE";
    default: return {};
  }
}

bool is_informational(const HttpStatusCode& code) {
  return (code >= 100 && code < 200);
}

bool is_successful(const HttpStatusCode& code) {
  return (code >= 200 && code < 300);
}

bool is_redirection(const HttpStatusCode& code) {
  return (code >= 300 && code < 400);
}

bool is_client_error(const HttpStatusCode& code) {
  return (code >= 400 && code < 500);
}

bool is_server_error(const HttpStatusCode& code) {
  return (code >= 500);
}

bool is_error(const HttpStatusCode& code) {
  return (code >= 400);
}

std::string to_string(const HttpStatusCode& code) {
	switch (code) {
  	case HttpStatusCode::Continue: return "Continue";
  	case HttpStatusCode::SwitchingProtocols: return "Switching Protocols";
  	case HttpStatusCode::Processing: return "Processing";
  	case HttpStatusCode::EarlyHints: return "Early Hints";

  	case HttpStatusCode::OK: return "OK";
  	case HttpStatusCode::Created: return "Created";
  	case HttpStatusCode::Accepted: return "Accepted";
  	case HttpStatusCode::NonAuthoritativeInformation: return "Non-Authoritative Information";
  	case HttpStatusCode::NoContent: return "No Content";
  	case HttpStatusCode::ResetContent: return "Reset Content";
  	case HttpStatusCode::PartialContent: return "Partial Content";
  	case HttpStatusCode::MultiStatus: return "Multi-Status";
  	case HttpStatusCode::AlreadyReported: return "Already Reported";
  	case HttpStatusCode::IMUsed: return "IM Used";

  	case HttpStatusCode::MultipleChoices: return "Multiple Choices";
  	case HttpStatusCode::MovedPermanently: return "Moved Permanently";
  	case HttpStatusCode::Found: return "Found";
  	case HttpStatusCode::SeeOther: return "See Other";
  	case HttpStatusCode::NotModified: return "Not Modified";
  	case HttpStatusCode::UseProxy: return "Use Proxy";
  	case HttpStatusCode::TemporaryRedirect: return "Temporary Redirect";
  	case HttpStatusCode::PermanentRedirect: return "Permanent Redirect";

  	case HttpStatusCode::BadRequest: return "Bad Request";
  	case HttpStatusCode::Unauthorized: return "Unauthorized";
  	case HttpStatusCode::PaymentRequired: return "Payment Required";
  	case HttpStatusCode::Forbidden: return "Forbidden";
  	case HttpStatusCode::NotFound: return "Not Found";
  	case HttpStatusCode::MethodNotAllowed: return "Method Not Allowed";
  	case HttpStatusCode::NotAcceptable: return "Not Acceptable";
  	case HttpStatusCode::ProxyAuthenticationRequired: return "Proxy Authentication Required";
  	case HttpStatusCode::RequestTimeout: return "Request Timeout";
  	case HttpStatusCode::Conflict: return "Conflict";
  	case HttpStatusCode::Gone: return "Gone";
  	case HttpStatusCode::LengthRequired: return "Length Required";
  	case HttpStatusCode::PreconditionFailed: return "Precondition Failed";
  	case HttpStatusCode::ContentTooLarge: return "Content Too Large";
  	case HttpStatusCode::URITooLong: return "URI Too Long";
  	case HttpStatusCode::UnsupportedMediaType: return "Unsupported Media Type";
  	case HttpStatusCode::RangeNotSatisfiable: return "Range Not Satisfiable";
  	case HttpStatusCode::ExpectationFailed: return "Expectation Failed";
  	case HttpStatusCode::ImATeapot: return "I'm a teapot";
  	case HttpStatusCode::MisdirectedRequest: return "Misdirected Request";
  	case HttpStatusCode::UnprocessableContent: return "Unprocessable Content";
  	case HttpStatusCode::Locked: return "Locked";
  	case HttpStatusCode::FailedDependency: return "Failed Dependency";
  	case HttpStatusCode::TooEarly: return "Too Early";
  	case HttpStatusCode::UpgradeRequired: return "Upgrade Required";
  	case HttpStatusCode::PreconditionRequired: return "Precondition Required";
  	case HttpStatusCode::TooManyRequests: return "Too Many Requests";
  	case HttpStatusCode::RequestHeaderFieldsTooLarge: return "Request Header Fields Too Large";
  	case HttpStatusCode::UnavailableForLegalReasons: return "Unavailable For Legal Reasons";

  	case HttpStatusCode::InternalServerError: return "Internal Server Error";
  	case HttpStatusCode::NotImplemented: return "Not Implemented";
  	case HttpStatusCode::BadGateway: return "Bad Gateway";
  	case HttpStatusCode::ServiceUnavailable: return "Service Unavailable";
  	case HttpStatusCode::GatewayTimeout: return "Gateway Timeout";
  	case HttpStatusCode::HTTPVersionNotSupported: return "HTTP Version Not Supported";
  	case HttpStatusCode::VariantAlsoNegotiates: return "Variant Also Negotiates";
  	case HttpStatusCode::InsufficientStorage: return "Insufficient Storage";
  	case HttpStatusCode::LoopDetected: return "Loop Detected";
  	case HttpStatusCode::NotExtended: return "Not Extended";
  	case HttpStatusCode::NetworkAuthenticationRequired: return "Network Authentication Required";

  	default: return std::string();
	}
}

Response::Response(const std::string& body)
: body(body), code(HttpStatusCode::OK) {
  set_default_header_fields(header);
  append_header("Content-Length", std::to_string(body.size()));
}

Response::Response(const HttpStatusCode& code)
: body(""), code(code) {
  set_default_header_fields(header);
  append_header("Content-Length", std::to_string(0));
}

Response::Response(const std::string& body, const HttpStatusCode& code)
: body(body), code(code) {
  set_default_header_fields(header);
  append_header("Content-Length", std::to_string(body.size()));
}

void Response::append_header(const std::string& key, const std::string& value) {
  header.insert({key, value});
}
void Response::set_body(const std::string& vbody) {
  body = vbody;
  append_header("Content-Length", std::to_string(body.size()));
}

void Response::set_code(const HttpStatusCode& vcode) {
  code = vcode;
}

std::string Response::to_string() const {
  return std::format(
    "HTTP/1.1 {} {}\n{}\r\n{}",
    static_cast<int>(code),
    http::to_string(code),
    header_to_string(header),
    body
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
  for(const auto& [k, v]: header)
    res += std::format("{}: {}\n", k, v);
  return res;
}
