project(
  'RandomArt',
  'c',
  license: 'MIT',
  license_files: 'LICENSE',
  version: '0.1.0'
)

subdir('src')
inc = include_directories('inc')

add_project_arguments('-DDEBUG', language : 'c')

cc = meson.get_compiler('c')
math_dep = cc.find_library('m', required : false)

executable(
  'randomart',
  sources: src,
  include_directories: inc,
  dependencies: math_dep
)
