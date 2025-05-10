#include <logger.h>
#include <print>

namespace http {
static LogLevel level;

void loglevel(const LogLevel& vlevel) {
  level = vlevel;
}

void info(const std::string& msg) {
  if(level <= LogLevel::INFO) {
    std::println("[INFO] {}", msg);
  }
}

void warning(const std::string& msg) {
  if(level <= LogLevel::WARNING) {
    std::println("[WARN] {}", msg);
  }
}

void debug(const std::string& msg) {
  if(level <= LogLevel::DEBUG) {
    std::println("[DEBUG] {}", msg);
  }
}

void error(const std::string& msg) {
  std::println("[ERROR] {}", msg);
}
}
