project(
  'lime',
  'cpp',
  version: run_command(
    ['bash', 'version'],
    check: true,
  ).stdout().strip(),
  default_options: [
    'cpp_std=c++23',
    'cpp_flags=-Wall -Werror -Wextra',
  ],
)
project_description = 'simple and lightweight web framework for C++'

incdir = include_directories('include')
srcs = files(
  'src/clime/http/methods.cc',
  'src/clime/http/request.cc',
  'src/clime/http/response.cc',
  'src/clime/http/router.cc',
  'src/clime/http/server.cc',
  'src/clime/json/json.cc',
  'src/lime/http/methods.cc',
  'src/lime/http/request.cc',
  'src/lime/http/response.cc',
  'src/lime/http/router.cc',
  'src/lime/http/server.cc',
  'src/lime/http/status.cc',
  'src/lime/json/json.cc',
  'src/lime/json/parser.cc',
  'src/lime/threadpool/threadpool.cc',
  'src/lime/utils/logger.cc',
)

lime = library(
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
    'include/lime/version.h',
    subdir: meson.project_name() + '/lime/',
  )

  install_headers(
    'include/clime/clime.h',
    'include/clime/version.h',
    subdir: meson.project_name() + '/clime/',
  )

  install_headers(
    'include/lime/http/http.h',
    'include/lime/http/methods.h',
    'include/lime/http/request.h',
    'include/lime/http/response.h',
    'include/lime/http/router.h',
    'include/lime/http/server.h',
    'include/lime/http/status.h',
    subdir: meson.project_name() + '/lime/http/',
  )

  install_headers(
    'include/clime/http/http.h',
    'include/clime/http/methods.h',
    'include/clime/http/request.h',
    'include/clime/http/response.h',
    'include/clime/http/router.h',
    'include/clime/http/server.h',
    'include/clime/http/status.h',
    subdir: meson.project_name() + '/clime/http/',
  )

  install_headers(
    'include/lime/json/json.h',
    subdir: meson.project_name() + '/lime/json/',
  )

  install_headers(
    'include/clime/json/json.h',
    subdir: meson.project_name() + '/clime/json/',
  )

  install_headers(
    'include/lime/threadpool/threadpool.h',
    subdir: meson.project_name() + '/lime/threadpool/',
  )

  install_headers(
    'include/lime/utils/logger.h',
    subdir: meson.project_name() + '/lime/utils/',
  )
endif

lime_dep = declare_dependency(
  include_directories: include_directories('include'),
  link_with: lime,
)
