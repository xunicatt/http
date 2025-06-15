project(
  'json_basics',
  'cpp',
  default_options: [
    'cpp_std=c++23',
    'cpp_flags=-Wall -Werror -Wextra',
  ],
)

http_dep = dependency('http', required: true)

executable(
  meson.project_name(),
  'main.cc',
  dependencies: [
    http_dep,
  ],
)
