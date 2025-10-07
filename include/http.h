#ifndef __HTTP_H__
#define __HTTP_H__

#include <format>

#include "logger.h"
#include "types.h"
#include "router.h"
#include "server.h"
#include "json.h"

#define __HTTP_VERSION_MAJ__ 1
#define __HTTP_VERSION_MIN__ 3
#define __HTTP_VERSION_PTC__ 7

namespace http {
[[nodiscard]]
constexpr inline std::string version() {
  return std::format(
    "v{}.{}.{}",
    __HTTP_VERSION_MAJ__,
    __HTTP_VERSION_MIN__,
    __HTTP_VERSION_PTC__
  );
}
} // http namespace end

#endif
