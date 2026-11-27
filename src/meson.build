fs = import('fs')

setup_sources = ['setup/setup.c', 'log/log.c', 'store/store.c']
if fs.exists('colour/color.c')
  setup_sources += 'colour/color.c'
endif

setup_lib = static_library(
  'setup',
  setup_sources,
)

stats_lib = static_library(
  'stats',
  'stats/stats.c',
)

tingone_deps = declare_dependency(
  include_directories : include_directories('.', 'setup', 'store', 'stats', 'log', 'colour'),
  link_with : [setup_lib, stats_lib],
)

executable('ting', 'main.c', dependencies : tingone_deps, install : true)
