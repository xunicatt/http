#include <logger.h>
#include <print>

namespace http {
static LogLevel level = LogLevel::Info;

void loglevel(const LogLevel& vlevel) {
  level = vlevel;
}

void info(const std::string& msg) {
  if(level <= LogLevel::Info) {
    std::println("[INFO] {}", msg);
  }
}

void warning(const std::string& msg) {
  if(level <= LogLevel::Warning) {
    std::println("[WARN] {}", msg);
  }
}

void debug(const std::string& msg) {
  if(level <= LogLevel::Debug) {
    std::println("[DEBUG] {}", msg);
  }
}

void error(const std::string& msg) {
  std::println(stderr, "[ERROR] {}", msg);
}
}
