test_inc = include_directories('.')
cc = meson.get_compiler('c')
util_dep = cc.find_library('util', required : false)

test_stats = executable('test_stats', 'test_stats.c',
                        dependencies : tingone_deps,
                        include_directories : test_inc)
test('stats', test_stats)

test_setup = executable('test_setup', 'test_setup.c',
                        dependencies : tingone_deps,
                        include_directories : test_inc)
test('setup', test_setup)

test_store = executable('test_store', 'test_store.c',
                        dependencies : tingone_deps,
                        include_directories : test_inc)
test('store', test_store)

test_log = executable('test_log', 'test_log.c',
                      dependencies : tingone_deps,
                      include_directories : test_inc,
                      link_args : ['-Wl,--wrap=time'])
test('log', test_log)

test_color = executable('test_color', 'test_color.c',
                        dependencies : [tingone_deps, util_dep],
                        include_directories : test_inc)
test('color-default', test_color, args : ['default'])
test('color-no-color', test_color, args : ['no_color'])
test('color-tty', test_color, args : ['tty'])

test_main = executable('test_main', ['test_main.c', '../src/main.c'],
                       c_args : ['-Dmain=ting_main'],
                       dependencies : tingone_deps,
                       include_directories : test_inc)
test('main', test_main)
