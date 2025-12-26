#ifndef LIME_JSON_PARSER_H
#define LIME_JSON_PARSER_H

#include <string>
#include <variant>
#include <cstdint>

#include "json.h"

namespace lime {
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
      size_t cursor = 0;
      size_t row = 0;
      size_t lnbeg = 0;
    };

    class Scanner {
    public:
      Scanner(const std::string&);
      template <typename T>
      [[nodiscard]]
      T& get();
      [[nodiscard]]
      std::expected<Token, std::string> token();
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

      std::variant<int64_t, double, bool, std::string> m_value;
      const std::string& m_data;
      ScannerLocation    m_loc;
      ScannerLocation    m_lastloc;
    };

    template <typename T>
    inline T& Scanner::get() {
      return std::get<T>(m_value);
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

      Scanner& m_sc;
      Token    m_token;
    };
  } // json
} // lime

#endif // LIME_JSON_PARSER_H
