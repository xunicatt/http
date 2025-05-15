project(
  'json',
  'cpp',
  default_options: [
    'cpp_std=c++23',
    'cpp_flags=-Wall -Werror -Wextra',
    'cpp_args=-DHTTP_EXPERIMENTAL_MODULES',
  ],
)

http_dep = dependency(
  'http',
  default_options: [
    'cpp_args=-DHTTP_EXPERIMENTAL_MODULES',
  ],
  required: true,
)

executable(
  meson.project_name(),
  'main.cc',
  dependencies: [http_dep],
)
