project('heat_equation', 'cpp',
  version : '1.0.0',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

sdl2_dep = dependency('sdl2', fallback: ['sdl2', 'sdl2_dep'])
sdl2_ttf_dep = dependency('SDL2_ttf', fallback: ['sdl2_ttf', 'sdl2_ttf_dep'])

# Include subdirectories
subdir('lib')
subdir('src')
subdir('bin')
subdir('test')
subdir('doc')