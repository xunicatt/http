project(
  'json_basics',
  'cpp',
  default_options: [
    'cpp_std=c++23',
    'cpp_flags=-Wall -Werror -Wextra',
  ],
)

lime_dep = dependency('lime', required: true)

executable(
  meson.project_name(),
  'main.cc',
  dependencies: [ lime_dep ],
)
