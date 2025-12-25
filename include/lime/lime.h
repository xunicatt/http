/*
*
*    _ _
*    | (_)_ __ ___   ___
*    | | | '_ ` _ \ / _ \
*    | | | | | | | |  __/
*    |_|_|_| |_| |_|\___|
*
*    [Li]ght Weight
*    [M]ulti Threaded
*    & [E]fficient
*
*/


#ifndef LIME_H
#define LIME_H

#include "http/http.h"
#include "json/json.h"
#include "threadpool/threadpool.h"
#include "utils/logger.h"

namespace lime {
  namespace version {
    inline constexpr uint8_t Major = 2;
    inline constexpr uint8_t Minor = 0;
    inline constexpr uint8_t Patch = 0;
    inline constexpr uint8_t Roll  = 0;

    [[nodiscard]]
    constexpr inline std::string to_string() {
      if (Roll == 0) {
        return std::format("v{}.{}.{}", Major, Minor, Patch);
      }

      return std::format("v{}.{}.{}.{} (rolling)", Major, Minor, Patch, Roll);
    }
  } // verion
} // lime

#endif // LIME_H
