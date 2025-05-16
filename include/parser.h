#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__

#ifdef HTTP_EXPERIMENTAL_MODULES

#include <string>
#include <variant>

#include "json.h"

namespace json {
enum class Token {
  LCUR,
  RCUR,
  LSQR,
  RSQR,
  COLN,
  COMA,
  INTL,
  FLTL,
  BOLL,
  STRL,
  TEOF,
  NONE,
  TERR,
};

[[nodiscard]]
std::string to_string(const Token&);

class Scanner {
public:
  Scanner(const std::string&);
  template <typename T>
  [[nodiscard]]
  T& get();
  [[nodiscard]]
  Token token();

private:
  [[nodiscard]]
  bool is_end() const;
  [[nodiscard]]
  bool skip_whitespaces();
  [[nodiscard]]
  char curr_char() const;

  std::variant<int, float, bool, std::string> value;
  const std::string& data;
  size_t cursor;
};

template <typename T>
T& Scanner::get() {
  return std::get<T>(value);
}

class Parser {
public:
  Parser(Scanner&);
  [[nodiscard]]
  std::expected<Node, std::string> parse(bool fetch = true);

private:
  [[nodiscard]]
  std::expected<Node, std::string> object();
  [[nodiscard]]
  std::expected<Node, std::string> array();
  [[nodiscard]]
  std::expected<Node, std::string> literal();

  Scanner& sc;
  Token token;
};
}

#endif
#endif
