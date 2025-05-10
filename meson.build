project(
  'http',
  'cpp',
  version: '1.0.0',
  default_options: [
    'cpp_std=c++23',
    'cpp_flags=-Wall -Werror -Wextra',
  ],
)
project_description = 'simple and lightweight web framework for C++'

incdir = include_directories('include')
srcs = files(
  'libs/logger.cc',
  'libs/router.cc',
  'libs/server.cc',
  'libs/types.cc',
)

http = static_library(
  meson.project_name(),
  srcs,
  include_directories: incdir,
  install: true,
)

pkg_mod = import('pkgconfig')
pkg_mod.generate(
  name: meson.project_name(),
  description: project_description,
  version: meson.project_version(),
  libraries: http,
  subdirs: 'include',
  install_dir: get_option('libdir') / 'pkgconfig',
)

install_headers(
  'include/http.h',
  'include/logger.h',
  'include/methods.h',
  'include/router.h',
  'include/server.h',
  'include/status.h',
  'include/types.h',
  subdir: meson.project_name(),
)
