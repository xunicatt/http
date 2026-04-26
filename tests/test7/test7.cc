#include <cstring>
#include <http.h>
#include <print>
#include <string>
#include <vector>

namespace json = http::json;

struct SimpleData: json::Encode {
  int id;
  std::string name;

  SimpleData(int id, std::string name): id(id), name(name) {}

  json::Node encode() const override {
    return json::Object{
      { "id", id },
      { "name", name },
    };
  }
};

struct ComplexData: json::Encode {
  SimpleData data;
  std::vector<std::string> skills;
  std::vector<std::string> languages;

  ComplexData(
    int id,
    std::string name,
    std::vector<std::string> skills,
    std::vector<std::string> languages
  ) : data({id, name}), skills(skills), languages(languages) {}

  json::Node encode() const override {
    std::vector<json::Node> skills_node, languages_node;

    skills_node.reserve(skills.size());
    languages_node.reserve(languages.size());

    for (const auto& skill: skills) {
      skills_node.emplace_back(skill);
    }

    for (const auto& language: languages) {
      languages_node.emplace_back(language);
    }

    return json::Object{
      { "person", data.encode() },
      { "data", json::Object{
        { "skills", skills_node },
        { "languages", languages_node },
      }}
    };
  }
};

int main() {
  const ComplexData data {
    123, "John Smith",
    { "Painting", "Programming" },
    { "C", "C++", "Rust" }
  };
  http::Router router;

  router.add("/", http::Method::Get, [&data](const http::Request&) {
    return http::Response(json::encode(data));
  });

  http::Server server { router };
  if (server.run() < 0) {
    std::println("ERROR: {}", strerror(errno));
    return 1;
  }
}
