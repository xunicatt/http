#include <parser.h>
#include <format>

namespace json = http::json;

static std::unexpected<std::string>
fmterror(const std::string&, const json::ScannerLocation&);

namespace http {
namespace json {
std::string to_string(const Token& t) {
  switch(t) {
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
    case Token::EndOfFile:    return "end of fike";
    default:                  return {};
  }
}

ScannerLocation::ScannerLocation()
: cursor(0), row(0), lnbeg(0) {}

Scanner::Scanner(const std::string& data)
: data(data) {}

bool Scanner::is_end() const {
  return loc.cursor >= data.length();
}

void Scanner::forward() {
  if(!is_end() && loc.cursor++ &&  !is_end() && curr_char() == '\n') {
    loc.row++;
    loc.lnbeg = loc.cursor + 1;
  }
}

bool Scanner::skip_whitespaces() {
  while(!is_end() && std::isspace(curr_char()))
    forward();
  return is_end();
}

char Scanner::curr_char() const {
  if(is_end()) return 0;
  return data[loc.cursor];
}

const ScannerLocation& Scanner::location() const {
  return lastloc;
}

Token Scanner::token() {
  if(is_end()) return Token::EndOfFile;
  if(skip_whitespaces()) return Token::EndOfFile;

  lastloc = loc;

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

  if(std::isdigit(curr_char())) {
    bool is_float = false;
    while(!is_end() && (std::isdigit(curr_char()) || curr_char() == '.')) {
      if(!is_float && curr_char() == '.') {
        is_float = true;
      }
      forward();
    }

    const auto& number = data.substr(
      lastloc.cursor,
      loc.cursor - lastloc.cursor
    );
    if(is_float) {
      value = std::stof(number);
      return Token::Float;
    }

    value = std::stoi(number);
    return Token::Int;
  }

  if(std::isalpha(curr_char())) {
    while(!is_end() && std::isalpha(curr_char()))
      forward();

    auto const& ident = data.substr(
      lastloc.cursor,
      loc.cursor - lastloc.cursor
    );
    if(ident != "true" && ident != "false")
      return Token::Invalid;

    value = ident == "true";
    return Token::Bool;
  }

  // TODO: error handling with \n in string
  // BUG: above mentioned todo might cause some
  // kind of weired undefined behavious
  if(curr_char() == '"') {
    forward();
    while(!is_end() && curr_char() != '"')
      forward();

    auto const& str = data.substr(
      lastloc.cursor + 1,
      loc.cursor - lastloc.cursor - 1
    );
    value = str;
    forward();
    return Token::String;
  }

  return Token::Invalid;
}

Parser::Parser(Scanner& sc)
: sc(sc) {}

std::expected<Node, std::string> Parser::literal() {
  switch(token) {
    case Token::Int:    return sc.get<int>();
    case Token::Float:  return sc.get<float>();
    case Token::Bool:   return sc.get<bool>();
    case Token::String: return sc.get<std::string>();
    default:            return fmterror("expected a literal", sc.location());
  }
}

std::expected<Node, std::string> Parser::array() {
  Array array;
  if(token = sc.token(); token == Token::RightBracket) {
    return array;
  }

  while(true) {
    const auto& node = parse(false);
    if(!node.has_value()) return node;
    array.emplace_back(node.value());
    if(token = sc.token(); token == Token::RightBracket) break;
    if(token != Token::Comma) {
      return fmterror("expected ',' or ']'", sc.location());
    }
    token = sc.token();
  }

  return array;
}

std::expected<Node, std::string> Parser::object() {
  Object object;
  if(token = sc.token(); token == Token::RightBrace) {
    return object;
  }

  while(true) {
    if(token != Token::String) {
      return fmterror("expected 'string' literal", sc.location());
    }

    const std::string key = sc.get<std::string>();
    if(token = sc.token(); token != Token::Colon) {
      return fmterror("expected ':'", sc.location());
    }

    const auto& node = parse();
    if(!node.has_value()) return node;

    object.insert({key, node.value()});
    if(token = sc.token(); token == Token::RightBrace) {
      break;
    }

    if(token != Token::Comma) {
      return fmterror("expected ',' or '}'", sc.location());
    }

    token = sc.token();
  }

  return object;
}

std::expected<Node, std::string> Parser::parse(bool fetch) {
  if(fetch)
    token = sc.token();

  switch(token) {
    case Token::LeftBrace:
      return object();

    case Token::LeftBracket:
      return array();

    default:
      return literal();
  }
}
} // json namespace end
} // http namespace end

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
