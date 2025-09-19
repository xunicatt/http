#include <parser.h>
#include <map>
#include <format>

namespace json = http::json;

static const std::map<char, json::Token> token_table = {
  {'{', json::Token::LCUR},
  {'}', json::Token::RCUR},
  {'[', json::Token::LSQR},
  {']', json::Token::RSQR},
  {':', json::Token::COLN},
  {',', json::Token::COMA},
};

static std::unexpected<std::string>
fmterror(const std::string&, const json::ScannerLocation&);

namespace http {
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
: data(data), loc({0}), lastloc({0}) {}

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
  if(is_end()) return Token::TEOF;
  if(skip_whitespaces()) return Token::TEOF;

  lastloc = loc;

  if(token_table.contains(curr_char())) {
    Token t = token_table.at(curr_char());
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
      return Token::FLTL;
    }

    value = std::stoi(number);
    return Token::INTL;
  }

  if(std::isalpha(curr_char())) {
    while(!is_end() && std::isalpha(curr_char()))
      forward();

    auto const& ident = data.substr(
      lastloc.cursor,
      loc.cursor - lastloc.cursor
    );
    if(ident != "true" && ident != "false")
      return Token::TERR;

    value = ident == "true";
    return Token::BOLL;
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
    default: return fmterror("expected a literal", sc.location());
  }
}

std::expected<Node, std::string> Parser::array() {
  Array array;
  if(token = sc.token(); token == Token::RSQR) {
    return array;
  }

  while(true) {
    const auto& node = parse(false);
    if(!node.has_value()) return node;
    array.emplace_back(node.value());
    if(token = sc.token(); token == Token::RSQR) break;
    if(token != Token::COMA) {
      return fmterror("expected ',' or ']'", sc.location());
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

  while(true) {
    if(token != Token::STRL) {
      return fmterror("expected 'string' literal", sc.location());
    }

    const std::string key = sc.get<std::string>();
    if(token = sc.token(); token != Token::COLN) {
      return fmterror("expected ':'", sc.location());
    }

    const auto& node = parse();
    if(!node.has_value()) return node;

    object.insert({key, node.value()});
    if(token = sc.token(); token == Token::RCUR) {
      break;
    }

    if(token != Token::COMA) {
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
    case Token::LCUR:
      return object();

    case Token::LSQR:
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
