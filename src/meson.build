fs = import('fs')

setup_sources = ['setup.c', 'store.c']
if fs.exists('color.c')
  setup_sources += 'color.c'
endif

setup_lib = static_library(
  'setup',
  ['setup.c', 'log.c'],
)

stats_lib = static_library(
  'stats',
  'stats.c',
)

tingone_deps = declare_dependency(
  include_directories : include_directories('.'),
  link_with : [setup_lib, stats_lib],
)

executable('ting', 'main.c', dependencies : tingone_deps, install : true)