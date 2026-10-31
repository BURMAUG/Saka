test_inc = include_directories('.')

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