cmake_minimum_required(VERSION 3.14)
project(lime VERSION 2.0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Werror)

set(
  SOURCES
  src/http/methods.cc
  src/http/request.cc
  src/http/response.cc
  src/http/router.cc
  src/http/server.cc
  src/http/status.cc
  src/json/json.cc
  src/json/parser.cc
  src/threadpool/threadpool.cc
  src/utils/logger.cc
)

add_library(lime STATIC ${SOURCES})
target_include_directories(lime PUBLIC include)

# Install library and headers
install(
  TARGETS lime
  EXPORT limeTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  DIRECTORY include
  DESTINATION include
  FILES_MATCHING PATTERN "*.h"
)

# Generate pkg-config file
configure_file(lime.pc.in lime.pc @ONLY)
install(
  FILES ${CMAKE_BINARY_DIR}/lime.pc
  DESTINATION lib/pkgconfig
)
