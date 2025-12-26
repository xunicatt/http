#include <lime/lime.h>
#include <print>

namespace lime {
  static LogLevel s_level { LogLevel::Info };

  void loglevel(const LogLevel& level) {
    s_level = level;
  }

  void info(const std::string& msg) {
    if (s_level <= LogLevel::Info) {
      std::println("[INFO] {}", msg);
    }
  }

  void warning(const std::string& msg) {
    if (s_level <= LogLevel::Warning) {
      std::println("[WARN] {}", msg);
    }
  }

  void debug(const std::string& msg) {
    if (s_level <= LogLevel::Debug) {
      std::println("[DEBUG] {}", msg);
    }
  }

  void error(const std::string& msg) {
    std::println(stderr, "[ERROR] {}", msg);
  }
} // lime
