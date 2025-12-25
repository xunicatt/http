# lime [WIP]
lime is a lightweight, multi-threaded HTTP library written in C++, designed for simplicity and ease of integration. It facilitates handling HTTP requests and responses with no dependencies, making it ideal for applications requiring a straightforward HTTP interface.

This library began as a personal exploration into how HTTP and HTTP parsing actually work under the hood. It isn’t designed for production use, there are plenty of mature and well-optimized HTTP libraries for C++ already. Instead, this project is a simple, hobby-level implementation built using basic TCP sockets, created purely for fun and for learning something new.

I wrote it without looking at how other HTTP libraries are implemented, so parts of it may be unconventional (or even wrong!), but that’s part of the learning process. I continue to update it with small features and optimizations whenever I can.

If you find it useful for small home projects or want to tinker with the internals, feel free to try it out!

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
`lime::http::Router` allows `lime::http::Server` to handle multiple routes and also supports regex pattern matching to resolve them.

### Example
```cpp
lime::http::Router router;
router.add(
  "/",
  lime::http::Method::Get,
  [](const lime::http::Request& req) {
    return lime::http::Response("Hello, World!");
  }
);

// with regex
router.add_regex(
  "/user/.+$",
  lime::http::Method::Get,
  [](const lime::http::Request& req) {
    // get the last token in url path /user/.../<name>
    const auto username = req.segments().back();
    return lime::http::Response(std::format("Hi {}", username));
  }
);

// with parameters
// uname=xunicatt
router.add(
  "/user",
  lime::http::Method::Get,
  [](const lime::http::Request& req) {
    if(!req.params.contains("uname")) {
      return lime::http::Response(lime::http::StatusCode::BadRequest);
    }

    const auto& uname = req.param.at("uname");
    return lime::http::Response(std::format("Hi {}", uname));
  }
);
```

## Server
`lime::http::Server` accepts an `lime::http::Router` to handle various routes and manages clients using multiple threads. It is responsible for setting up the socket and binding the address and port.

### Example
```cpp
lime::http::Server server(router);
if(server.port(8080).run() < 0) {
  // error
  // errno is set
}
```

## Build & Install
### Requirements:
- C++23
- meson/cmake
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
To use `lime` as a meson subproject we have to make a project structure like this:
- meson.build
- main.cc
- subprojects/
  - lime/

```bash
mkdir <project-name>
cd $_
touch main.cc meson.build
mkdir subprojects
git clone https://github.com/xunicatt/lime.git ./subprojects/lime
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

lime = subproject('lime')

executable(
  meson.project_name(),
  'main.cc',
  dependencies: [
    lime.get_variable('lime_dep'),
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

add_subdirectory(subprojects/lime)
add_executable(example main.cc)
target_link_libraries(example PRIVATE lime)
```

main.cc:
```cpp
#include <lime/lime.h>
#include <cstring>

int main() {
  namespace http = lime::http;

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
- json (stable): available directly by using "lime::json" namespace
- threadpool (stable): available directly by using "lime::DynamicThreadPool"

## Experimental Modules
> [!WARNING]
> EXPERIMENTAL MODULES ARE NOT READY FOR GENERAL USE. THESE MODULES MAY HAVE INCOMPLETE FEATURES AND ARE SUBJECT TO CHANGE OR BREAK AT ANY TIME.

## Docs
Checkout `example/` directory for project integration and example.

> [!NOTE]
> This project was previously named http and has been renamed to lime to avoid generic naming conflicts and to better reflect the broader scope of the codebase. As the project grew beyond a simple HTTP experiment to include additional modules such as JSON and a threadpool, keeping everything under the http name and namespace became limiting. This rename introduces breaking changes, including updated namespaces, include paths, build targets, and a reorganized project structure. The internal threadpool is now used as the default client handler. HTTP remains the primary focus of the project, with future releases planned to add features such as templating, HTTPS support, and an HTTP client.
