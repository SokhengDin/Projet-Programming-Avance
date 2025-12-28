sdl_sources = files(
  'sdl_core.cpp',
  'sdl_window.cpp',
  'sdl_heatmap.cpp',
  'sdl_font.cpp',
  'sdl_app.cpp'
)

sdl_inc = include_directories('.')

sdl_lib = static_library('sdl_wrapper',
  sdl_sources,
  include_directories : [sdl_inc, include_directories('../heat')],
  dependencies : [sdl2_dep, sdl2_ttf_dep],
  install : false
)

sdl_dep = declare_dependency(
  link_with : sdl_lib,
  include_directories : [sdl_inc, include_directories('../heat')],
  dependencies : [sdl2_dep, sdl2_ttf_dep]
)
