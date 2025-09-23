# http [WIP]
http is a lightweight, multi-threaded HTTP library written in C++, designed for simplicity and ease of integration. It facilitates handling HTTP requests and responses with no dependencies, making it ideal for applications requiring a straightforward HTTP interface.

> [!WARNING]
> THIS LIBRARY IS A WORK IN PROGRESS AND IS CURRENTLY IN THE EARLY STAGES.

# Features
- Routing System: Define routes with specific HTTP methods and handlers.
- Request & Response Handling: Abstractions for incoming requests and structured responses.
- Status Codes: Complete enumeration of HTTP status codes with utilities.
- Logging: Customizable logging with support for debug, info, warning, and error levels.
- Server Control: Easily start and configure an HTTP server with port and address bindings.
- Json: Builtin json moudle allows easy encoding and decoding.

## Routing
`http::Router` allows `http::Server` to handle multiple routes and also supports regex pattern matching to resolve them.

### Example
```cpp
http::Router router;
router.add(
  "/",
  http::Method::Get,
  [](const http::Request& req) {
    return http::Response("Hello, World!");
  }
);

// with regex
router.add_regex(
  "/user/.+$",
  http::Method::Get,
  [](const http::Request& req) {
    // get the last token in url path /user/.../<name>
    const auto username = req.segments().back();
    return http::Response(std::format("Hi {}", username));
  }
);

// with parameters
// uname=xunicatt
router.add(
  "/user",
  http::Method::Get,
  [](const http::Request& req) {
    if(!req.params.contains("uname")) {
      return http::Response(http::HttpStatusCode::BadRequest);
    }

    const auto& uname = req.param.at("uname");
    return http::Response(std::format("Hi {}", uname));
  }
);
```

## Server
`http::Server` accepts an `http::Router` to handle various routes and manages clients using multiple threads. It is responsible for setting up the socket and binding the address and port.

### Example
```cpp
http::Server server(router);
if(server.port(8080).run() < 0) {
  // error
  // errno is set
}
```

## Build & Install
### Requirements:
- C++23
- meson
- linux/macos

### System Wide:
- meson
```bash
meson setup build
meson compile -C build
meson install -C build
```

- cmake
```bash
mkdir build && cd $_
cmake ..
sudo make install
```

### As a Meson Project:
To use `http` as a meson subproject we have to make a project structure like this:
- meson.build
- main.cc
- subprojects/
  - http/

```bash
mkdir <project-name>
cd $_
touch main.cc meson.build
mkdir subprojects
git clone https://github.com/xunicatt/http.git ./subprojects/http
```

meson.build:
```meson
project(
  'example',
  'cpp',
  default_options: [
    'cpp_std=c++23',
    'cpp_flags=-Wall -Wextra -Werror',
  ],
)

http = subproject('http')

executable(
  meson.project_name(),
  'main.cc',
  dependencies: [
    http.get_variable('http_dep'),
  ],
)
```

or

CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.14)
project(example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Werror)

add_subdirectory(subprojects/http)
add_executable(example main.cc)
target_link_libraries(example PRIVATE http)
```

main.cc:
```cpp
#include <http.h>
#include <cstring>

int main() {
  http::Router router;
  router.add("/", http::Method::Get, [](const http::Request& req){
    return http::Response("Hello, World!");
  });

  http::Server server(router);
  if(server.port(8080).run() < 0) {
    perror(strerror(errno));
    return 1;
  }

  return 0;
}
```

Now to build and run it:

meson:
```bash
meson setup build
cd build
ninja
./example
```

cmake:
```bash
mkdir build && cd $_
cmake ..
make
./example
```

## Modules:
- json (stable): available directly by using "http::json" namespace

## Experimental Modules
> [!WARNING]
> EXPERIMENTAL MODULES ARE NOT READY FOR GENERAL USE. THESE MODULES MAY HAVE INCOMPLETE FEATURES AND ARE SUBJECT TO CHANGE OR BREAK AT ANY TIME.

To use experimental modules from 'http' in a meson subproject, we have to enable `HTTP_EXPERIMENTAL_MODULES` flag by using:
```bash
meson setup build -Dhttp:cpp_args="-DHTTP_EXPERIMENTAL_MODULES" -Dcpp_args="-DHTTP_EXPERIMENTAL_MODULES"
```

## Docs
Checkout `example/` directory for project integration and example.
