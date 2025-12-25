#include <lime/lime.h>

namespace lime {
  namespace http {
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

       	default: return {};
      }
    }
  } // http
} // lime
