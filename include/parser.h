#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__

#include <string>
#include <variant>

#include "json.h"

namespace http {
namespace json {
enum class Token {
  None,
  EndOfFile,
  Invalid,
  Int,
  Float,
  Bool,
  String,
  Colon,
  Comma,
  LeftBrace,
  RightBrace,
  LeftBracket,
  RightBracket,
};

[[nodiscard]]
std::string to_string(const Token&);

struct ScannerLocation {
  size_t cursor;
  size_t row;
  size_t lnbeg;

  ScannerLocation();
};

class Scanner {
public:
  Scanner(const std::string&);
  template <typename T>
  [[nodiscard]]
  T& get();
  [[nodiscard]]
  Token token();
  [[nodiscard]]
  const ScannerLocation& location() const;

private:
  void forward();
  [[nodiscard]]
  bool is_end() const;
  [[nodiscard]]
  bool skip_whitespaces();
  [[nodiscard]]
  char curr_char() const;
  [[nodiscard]]
  char peek_char() const;

  std::variant<int, float, bool, std::string> value;
  const std::string& data;
  ScannerLocation    loc;
  ScannerLocation    lastloc;
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
  Token    token;
};
} // json namespace end
} // http namespace end

#endif
