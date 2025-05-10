#ifndef __HTTP_LOGGER_H__
#define __HTTP_LOGGER_H__

#include <string>

namespace http {
enum LogLevel {
  NONE,
  DEBUG,
  WARNING,
  INFO,
};

void loglevel(const LogLevel&);
void info(const std::string&);
void warning(const std::string&);
void debug(const std::string&);
void error(const std::string&);
};

#endif
