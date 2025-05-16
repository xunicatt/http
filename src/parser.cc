#include <parser.h>

#ifdef HTTP_EXPERIMENTAL_MODULES

#include <map>
#include <print>

static const std::map<char, json::Token> token_table = {
  {'{', json::Token::LCUR},
  {'}', json::Token::RCUR},
  {'[', json::Token::LSQR},
  {']', json::Token::RSQR},
  {':', json::Token::COLN},
  {',', json::Token::COMA},
};

namespace json {
std::string to_string(const Token& t) {
  switch(t) {
    case Token::LCUR: return "{";
    case Token::RCUR: return "}";
    case Token::LSQR: return "[";
    case Token::RSQR: return "]";
    case Token::COLN: return ":";
    case Token::COMA: return ",";
    case Token::INTL: return "int";
    case Token::FLTL: return "float";
    case Token::BOLL: return "bool";
    case Token::STRL: return "string";
    case Token::TERR: return "error";
    case Token::TEOF: return "eof";
    default: return {};
  }
}

Scanner::Scanner(const std::string& data)
: data(data), cursor(0) {}

bool Scanner::is_end() const {
  return cursor >= data.length();
}

bool Scanner::skip_whitespaces() {
  while(!is_end() && std::isspace(data[cursor]))
    cursor++;
  return is_end();
}

char Scanner::curr_char() const {
  if(is_end()) return 0;
  return data[cursor];
}

Token Scanner::token() {
  if(is_end()) return Token::TEOF;
  if(skip_whitespaces()) return Token::TEOF;

  if(token_table.contains(curr_char())) {
    Token t = token_table.at(curr_char());
    cursor++;
    return t;
  }

  if(std::isdigit(curr_char())) {
    size_t beg = cursor;
    bool is_float = false;
    while(!is_end() && (std::isdigit(curr_char()) || curr_char() == '.')) {
      if(!is_float && curr_char() == '.') {
        is_float = true;
      }
      cursor++;
    }

    const auto& number = data.substr(beg, cursor - beg);
    if(is_float) {
      value = std::stof(number);
      return Token::FLTL;
    }

    value = std::stoi(number);
    return Token::INTL;
  }

  if(std::isalpha(curr_char())) {
    size_t beg = cursor;
    while(!is_end() && std::isalpha(curr_char()))
      cursor++;

    auto const& ident = data.substr(beg, cursor - beg);
    if(ident != "true" && ident != "false")
      return Token::TERR;

    value = ident == "true";
    return Token::BOLL;
  }

  if(curr_char() == '"') {
    size_t beg = cursor;
    cursor++;
    while(!is_end() && curr_char() != '"')
      cursor++;

    auto const& str = data.substr(beg + 1, cursor - beg - 1);
    value = str;
    cursor++;
    return Token::STRL;
  }

  return Token::TERR;
}

Parser::Parser(Scanner& sc)
: sc(sc) {}

std::expected<Node, std::string> Parser::literal() {
  switch(token) {
    case Token::INTL: return sc.get<int>();
    case Token::FLTL: return sc.get<float>();
    case Token::BOLL: return sc.get<bool>();
    case Token::STRL: return sc.get<std::string>();
    default: return std::unexpected("expected a literal");
  }
}

std::expected<Node, std::string> Parser::array() {
  Array array;
  if(token = sc.token(); token == Token::LSQR) {
    return array;
  }

  while(token != Token::LSQR) {
    const auto& node = parse(false);
    if(!node.has_value()) return node;
    array.push_back(node.value());
    if(token = sc.token(); token == Token::RSQR) break;
    if(token != Token::COMA) {
      return std::unexpected("expected ,");
    }
    token = sc.token();
  }

  return array;
}

std::expected<Node, std::string> Parser::object() {
  Object object;
  if(token = sc.token(); token == Token::RCUR) {
    return object;
  }

  while(token != Token::RCUR) {
    if(token != Token::STRL) {
      return std::unexpected("expected 'string' literal");
    }

    const std::string key = sc.get<std::string>();
    if(token = sc.token(); token != Token::COLN) {
      return std::unexpected("expected ':'");
    }

    const auto& node = parse();
    if(!node.has_value()) return node;

    object.insert({key, node.value()});
    if(token = sc.token(); token == Token::RCUR) {
      break;
    }

    if(token != Token::COMA) {
      return std::unexpected("expected ','");
    }

    token = sc.token();
  }

  return object;
}

std::expected<Node, std::string> Parser::parse(bool fetch) {
  if(fetch)
    token = sc.token();

  switch(token) {
    case Token::LCUR:
      return object();

    case Token::LSQR:
      return array();

    default:
      return literal();
  }
}
}

#endif
