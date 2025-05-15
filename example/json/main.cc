#include <print>
#include <cstring>
#include <http.h>
#include <json.h>

http::Response database(const http::Request& req) {
  /*
  // equivalent to:
  {
    "student": [
      {
        "name": "John",
        "id": 187
      },
      {
        "name": "Carol",
        "id": 122
      }
    ],
    "grades": [
      8.3,
      6.7
    ]
  }
  */

  json::Value value = json::Object{
    {"students", json::Array{
      json::Object{
        {"name", "John"},
        {"id", 187}
      },
      json::Object{
        {"name", "Carol"},
        {"roll", 122}
      }
    }},
    {"grades", json::Array{
      8.3,
      6.7
    }}
  };

  http::Response resp(json::encode(value));
  resp.append_header("Content-Type", "application/json");
  return resp;
}

int main() {
  http::Router router;
  router.add("/database", http::GET, database);
  http::Server server(router);
  if(server.run() < 0) {
    std::println("failed to run server: {}", strerror(errno));
    exit(EXIT_FAILURE);
  }
}
