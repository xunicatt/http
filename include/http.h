#ifndef __HTTP_H__
#define __HTTP_H__

#include <format>

#include "logger.h"
#include "types.h"
#include "router.h"
#include "server.h"
#include "json.h"

[[deprecated("Use http::ver::Major instead")]]
inline constexpr uint8_t __HTTP_VERSION_MAJ__ = 1;
[[deprecated("Use http::ver::Minor instead")]]
inline constexpr uint8_t __HTTP_VERSION_MIN__ = 3;
[[deprecated("Use http::ver::Patch instead")]]
inline constexpr uint8_t __HTTP_VERSION_PTC__ = 9;

namespace http {
namespace ver {
  inline constexpr uint8_t Major = 1;
  inline constexpr uint8_t Minor = 3;
  inline constexpr uint8_t Patch = 9;
  inline constexpr uint8_t Roll  = 1;

  [[nodiscard]]
  constexpr inline std::string to_string() {
    if (Roll == 0) {
      return std::format("v{}.{}.{}", Major, Minor, Patch);
    }

    return std::format("v{}.{}.{}+{}", Major, Minor, Patch, Roll);
  }
} // version namespace end

[[nodiscard, deprecated("Use http::ver::to_string instead")]]
constexpr inline std::string version() {
  return ver::to_string();
};
} // http namespace end

#endif
