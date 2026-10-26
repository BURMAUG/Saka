# Build pipeline and tests

## Why
The project previously had tests disabled (`#subdir('tests')`) and no automated
build/test pipeline. This change adds a self-contained test setup (no external
test framework, per the "make it yourself" policy) and a GitHub Actions CI
workflow so builds and tests run on every push/PR and locally with the same
commands.

## How it was made

### Build system
- `meson.build` now calls `subdir('tests')` so test executables are built.
- `src/meson.build` compiles `store.c` into `setup_lib` (it was previously
  empty and uncompiled).
- `tests/meson.build` registers three test executables via Meson's built-in
  `test()` function: `stats`, `setup`, and `store`.

### Test harness
- `tests/test_harness.h` provides minimal assertion macros (`ASSERT_TRUE`,
  `ASSERT_INT_EQ`, `ASSERT_STR_EQ`), a `RUN_TEST` runner, and a summary
  printer. No third-party dependencies are used.

### Testability refactors
- `src/setup.h` exposes pure helpers `parse_storage_buffer()` and
  `lookup_record()` so parsing/lookup can be tested without touching files.
- `src/setup.c` splits file I/O (read/write) from parsing logic and uses the
  `TING_STORE_FILE` environment variable to override the store path (useful for
  tests and for avoiding clobbering the real store).
- `src/store.c` implements file-descriptor helpers `read_fd_to_string()` and
  `write_string_fd()` to keep I/O fd-based, as requested.
- `src/setup.c` fixes the `memcpy` source bug in the old `extract_data` logic
  and avoids VLAs by using fixed `BUFF_SIZE` buffers.
- `src/main.c` now checks the return value of `setup_target_path()` instead of
  an (already closed) file descriptor.

### Running locally
```sh
meson setup build
ninja -C build
meson test -C build --print-errorlogs
```

### CI
`.github/workflows/ci.yml` runs on push to `master`, `main`, `develop`, and `feature/**`/`feat/**`/`bugfix/**`
branches, plus PRs to `master`/`main`:
1. Install Meson + Ninja (+ gcc/clang matrix).
2. `meson setup build`
3. `ninja -C build`
4. `meson test -C build --print-errorlogs`

### Data format
`STORE_FILE` lines use CSV: `path,frequency,duration,is_active`.
Parsing reads the trailing three numeric fields from the right so paths may
contain commas. CRLF line endings are accepted.