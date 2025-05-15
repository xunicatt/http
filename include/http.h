#ifndef __HTTP_H__
#define __HTTP_H__

#include <format>

#include "logger.h"
#include "types.h"
#include "router.h"
#include "server.h"

#ifdef HTTP_EXPERIMENTAL_MODULES
  #include "json.h"
#endif

#define __HTTP_VERSION_MAJ__ 1
#define __HTTP_VERSION_MIN__ 0
#define __HTTP_VERSION_PTC__ 4

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
}

#endif
