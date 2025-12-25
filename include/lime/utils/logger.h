#ifndef LIME_UTILS_LOGGER_H
#define LIME_UTILS_LOGGER_H

#include <string>

namespace lime {
  enum class LogLevel {
    Debug,
    Warning,
    Info,
    None,
  };

  void loglevel(const LogLevel&);
  void info(const std::string&);
  void warning(const std::string&);
  void debug(const std::string&);
  void error(const std::string&);
} // lime

#endif // LIME_UTILS_LOGGER_H
