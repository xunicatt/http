#include <expected>
#include <lime/json/parser.h>
#include <format>

namespace lime {
  namespace json {
    static std::unexpected<std::string>
    fmterror(const std::string&, const json::ScannerLocation&);

    std::string to_string(const Token& t) {
      switch (t) {
        case Token::LeftBrace:    return "{";
        case Token::RightBrace:   return "}";
        case Token::LeftBracket:  return "[";
        case Token::RightBracket: return "]";
        case Token::Colon:        return ":";
        case Token::Comma:        return ",";
        case Token::Int:          return "int";
        case Token::Float:        return "float";
        case Token::Bool:         return "bool";
        case Token::String:       return "string";
        case Token::Invalid:      return "invalid";
        case Token::EndOfFile:    return "end of file";
        default:                  return {};
      }
    }

    ScannerLocation::ScannerLocation()
    : cursor(0), row(0), lnbeg(0) {}

    Scanner::Scanner(const std::string& data)
    : m_data(data) {}

    bool Scanner::is_end() const {
      return m_loc.cursor >= m_data.length();
    }

    void Scanner::forward() {
      if (!is_end() && m_loc.cursor++ &&
        !is_end() && curr_char() == '\n') {
          m_loc.row++;
          m_loc.lnbeg = m_loc.cursor + 1;
        }
    }

    bool Scanner::skip_whitespaces() {
      while (!is_end() && std::isspace(curr_char())) {
        forward();
      }
      return is_end();
    }

    char Scanner::curr_char() const {
      if (is_end()) {
        return 0;
      }
      return m_data[m_loc.cursor];
    }

    char Scanner::peek_char() const {
      if (m_loc.cursor + 1 >= m_data.length()) {
        return 0;
      }
      return m_data[m_loc.cursor + 1];
    }

    const ScannerLocation& Scanner::location() const {
      return m_lastloc;
    }

    std::expected<Token, std::string> Scanner::token() {
      if (is_end()) {
        return Token::EndOfFile;
      }

      if (skip_whitespaces()) {
        return Token::EndOfFile;
      }

      m_lastloc = m_loc;

      Token t = Token::None;
      switch (curr_char()) {
        case '{':
        t = Token::LeftBrace;
        break;
        case '}':
        t = Token::RightBrace;
        break;
        case '[':
        t = Token::LeftBracket;
        break;
        case ']':
        t = Token::RightBracket;
        break;
        case ':':
        t = Token::Colon;
        break;
        case ',':
        t = Token::Comma;
        break;
      }

      if (t != Token::None) {
        forward();
        return t;
      }

      if (std::isdigit(curr_char()) ||
        (curr_char() == '-' && std::isdigit(peek_char()))) {
          bool is_float = false;
          if (curr_char() == '-') {
            forward();
          }

          while (!is_end() &&
            (std::isdigit(curr_char()) || curr_char() == '.')) {
              if (!is_float && curr_char() == '.') {
                is_float = true;
              }
              forward();
            }

          const auto& number = m_data.substr(
            m_lastloc.cursor,
            m_loc.cursor - m_lastloc.cursor
          );
          if (is_float) {
            m_value = std::stod(number);
            return Token::Float;
          }

          m_value = std::stoll(number);
          return Token::Int;
        }

      if (std::isalpha(curr_char())) {
        while (!is_end() && std::isalpha(curr_char())) {
          forward();
        }

        auto const& ident = m_data.substr(
          m_lastloc.cursor,
          m_loc.cursor - m_lastloc.cursor
        );
        if (ident != "true" && ident != "false") {
          return Token::Invalid;
        }

        m_value = ident == "true";
        return Token::Bool;
      }

      // TODO: handling escape characters like \n \t in string
      if (curr_char() == '"') {
        forward();
        while (!is_end() && curr_char() != '"') {
          if (curr_char() == '\n') {
            return fmterror("unexpected end of string", location());
          }
          forward();
        }

        auto const& str = m_data.substr(
          m_lastloc.cursor + 1,
          m_loc.cursor - m_lastloc.cursor - 1
        );
        m_value = str;
        forward();
        return Token::String;
      }

      return Token::Invalid;
    }

    Parser::Parser(Scanner& sc)
    : m_sc(sc) {}

    std::expected<Node, std::string> Parser::literal() {
      switch (m_token) {
        case Token::Int:    return m_sc.get<int64_t>();
        case Token::Float:  return m_sc.get<double>();
        case Token::Bool:   return m_sc.get<bool>();
        case Token::String: return m_sc.get<std::string>();
        default:            return fmterror("expected a literal", m_sc.location());
      }
    }

    std::expected<Node, std::string> Parser::array() {
      Array array;
      auto token = m_sc.token();

      if (!token) {
        return std::unexpected(token.error());
      }

      m_token = *token;
      if (*token == Token::RightBracket) {
        return array;
      }

      while (true) {
        const auto& node = parse(false);
        if (!node) {
          return node;
        }
        array.emplace_back(*node);

        if (!(token = m_sc.token())) {
          return std::unexpected(token.error());
        }

        m_token = *token;
        if (*token == Token::RightBracket) {
          break;
        }

        if (*token != Token::Comma) {
          return fmterror("expected ',' or ']'", m_sc.location());
        }

        if (!(token = m_sc.token())) {
          return std::unexpected(token.error());
        }
        m_token = *token;
      }

      return array;
    }

    std::expected<Node, std::string> Parser::object() {
      Object object;
      auto token = m_sc.token();
      if (!token) {
        return std::unexpected(token.error());
      }

      m_token = *token;
      if (*token == Token::RightBrace) {
        return object;
      }

      while (true) {
        if (*token != Token::String) {
          return fmterror("expected 'string' literal", m_sc.location());
        }

        const std::string key = m_sc.get<std::string>();
        if (!(token = m_sc.token())) {
          return std::unexpected(token.error());
        }

        m_token = *token;
        if (*token != Token::Colon) {
          return fmterror("expected ':'", m_sc.location());
        }

        const auto& node = parse();
        if (!node) {
          return node;
        }

        object.insert({ key, *node });

        if (!(token = m_sc.token())) {
          return std::unexpected(token.error());
        }

        m_token = *token;
        if (*token == Token::RightBrace) {
          break;
        }

        if (*token != Token::Comma) {
          return fmterror("expected ',' or '}'", m_sc.location());
        }

        if (!(token = m_sc.token())) {
          return std::unexpected(token.error());
        }
        m_token = *token;
      }

      return object;
    }

    std::expected<Node, std::string> Parser::parse(bool fetch) {
      if (fetch) {
        auto token = m_sc.token();
        if (!token) {
          return std::unexpected(token.error());
        }
        m_token = *token;
      }


      switch (m_token) {
        case Token::LeftBrace:
        return object();

        case Token::LeftBracket:
        return array();

        default:
        return literal();
      }
    }

    std::unexpected<std::string>
    fmterror(const std::string& msg, const json::ScannerLocation& loc) {
      return std::unexpected{
        std::format(
        "{}:{} {}",
        loc.row + 1,
        loc.cursor - loc.lnbeg + 1,
        msg
      )};
    }
  } // json
} // http
