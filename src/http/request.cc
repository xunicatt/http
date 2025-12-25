#include <vector>
#include <string>
#include <sstream>
#include <lime/lime.h>

namespace lime {
  namespace http {
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
  } // http
} // lime
