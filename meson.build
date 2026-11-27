project('tingone', 'c', version : '0.1.0')

subdir('src')
subdir('tests')

format_script = [find_program('python3'), files('scripts/format.py')]
run_target('format', command : format_script)
run_target('format-check', command : format_script + ['--check'])
