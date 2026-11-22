# GDB defaults for tingone (loaded when gdb is started in the project root)
set print pretty on
set print array on
# Point at the debug build by default
file build/src/ting
# Keep history between sessions
set history save on
set history filename .gdb_history