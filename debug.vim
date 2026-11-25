" Debug tingone from Vim using the built-in termdebug plugin.
" Usage:  vim -S debug.vim   (or :source debug.vim from inside Vim)
if !has('terminal')
  echoerr 'This Vim build has no terminal support; termdebug unavailable.'
  finish
endif

packadd termdebug
let g:termdebug_config = { 'wide': 163 }
TermdebugCommand build/src/ting
" Handy breakpoints for the CLI entry point
Break setup_target_path
Break main