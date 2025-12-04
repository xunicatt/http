project(
  'http',
  'cpp',
  version: '1.3.9.7',
  default_options: [
    'cpp_std=c++23',
    'cpp_flags=-Wall -Werror -Wextra',
  ],
)
project_description = 'simple and lightweight web framework for C++'

incdir = include_directories('include')
srcs = files(
  'src/json.cc',
  'src/logger.cc',
  'src/parser.cc',
  'src/router.cc',
  'src/server.cc',
  'src/types.cc',
  'src/threadpool.cc',
)

http = static_library(
  meson.project_name(),
  srcs,
  include_directories: incdir,
  install: true,
)

if not meson.is_subproject()
  pkg_mod = import('pkgconfig')
  pkg_mod.generate(
    name: meson.project_name(),
    description: project_description,
    version: meson.project_version(),
    libraries: http,
    subdirs: meson.project_name(),
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
    'include/json.h',
    'include/threadpool.h',
    subdir: meson.project_name(),
  )
endif

http_dep = declare_dependency(
  include_directories: include_directories('include'),
  link_with: http,
)
