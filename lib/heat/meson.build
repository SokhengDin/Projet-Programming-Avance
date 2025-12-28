# Heat equation solver library
heat_sources = files(
  'heat_equation_solver_1d.cpp',
  'heat_equation_solver_2d.cpp'
)

heat_inc = include_directories('.')

heat_lib = static_library('heat_solver',
  heat_sources,
  include_directories : heat_inc,
  install : false
)

heat_dep = declare_dependency(
  link_with : heat_lib,
  include_directories : heat_inc
)
