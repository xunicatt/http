project(
  'lime',
  'cpp',
  version: '2.0.0',
  default_options: [
    'cpp_std=c++23',
    'cpp_flags=-Wall -Werror -Wextra',
  ],
)
project_description = 'simple and lightweight web framework for C++'

incdir = include_directories('include')
srcs = files(
  'src/http/methods.cc',
  'src/http/request.cc',
  'src/http/response.cc',
  'src/http/router.cc',
  'src/http/server.cc',
  'src/http/status.cc',
  'src/json/json.cc',
  'src/json/parser.cc',
  'src/threadpool/threadpool.cc',
  'src/utils/logger.cc',
)

lime = static_library(
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
    libraries: lime,
    subdirs: meson.project_name(),
    install_dir: get_option('libdir') / 'pkgconfig',
  )

  install_headers(
    'include/lime/lime.h',
    subdir: meson.project_name(),
  )

  install_headers(
    'include/lime/http/http.h',
    'include/lime/http/methods.h',
    'include/lime/http/request.h',
    'include/lime/http/response.h',
    'include/lime/http/router.h',
    'include/lime/http/server.h',
    'include/lime/http/status.h',
    subdir: meson.project_name() + '/http/',
  )

  install_headers(
    'include/lime/json/json.h',
    subdir: meson.project_name() + '/json/',
  )

  install_headers(
    'include/lime/threadpool/threadpool.h',
    subdir: meson.project_name() + '/threadpool/',
  )

  install_headers(
    'include/lime/utils/logger.h',
    subdir: meson.project_name() + '/utils/',
  )
endif

lime_dep = declare_dependency(
  include_directories: include_directories('include'),
  link_with: lime,
)
