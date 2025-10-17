cmake_minimum_required(VERSION 3.14)
project(http VERSION 1.3.9+1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Werror)

set(
  SOURCES
  src/json.cc
  src/logger.cc
  src/parser.cc
  src/router.cc
  src/server.cc
  src/types.cc
)

add_library(http STATIC ${SOURCES})
target_include_directories(http PUBLIC include)

# Install library and headers
install(
  TARGETS http
  EXPORT httpTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  DIRECTORY include/
  DESTINATION include/http
  FILES_MATCHING PATTERN "*.h"
)

# Generate pkg-config file
configure_file(http.pc.in http.pc @ONLY)
install(
  FILES ${CMAKE_BINARY_DIR}/http.pc
  DESTINATION lib/pkgconfig
)
