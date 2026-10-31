# AGENTS.md

C/Meson CLI (`ting`) that tracks directory dwell time/frequency in a CSV store.

## Build & test

```sh
meson setup build
ninja -C build
meson test -C build --print-errorlogs
```

- Single suite: `meson test -C build setup` (also `stats`, `store`); or run `./build/tests/test_setup`.
- Reconfigure: `meson setup --reconfigure build`. Build dirs `build*/` are gitignored; delete a stale dir and re-setup if configure fails.
- CI compiles with both gcc and clang (`/.github/workflows/ci.yml`).

## Environment

- `TING_STORE_FILE=/path/to/log` overrides the store path (default `/home/burmo/.local/share/ting/log`). Use this in tests/dev to stay hermetic.
- `NO_COLOR=1` disables ANSI color (also auto-disabled off a TTY). Color state is cached on first use.

## Entrypoints / layout

- `src/main.c`: CLI verbs `ting cd <path>`, `ting ls|list|ds`, `ting stop|flush`.
- Persistence logic: `src/setup.c` + `src/store.c`. `src/stats.c` only provides `frequency()`.
- Logging helper: `src/log.c`/`src/log.h` (level-filtered, timestamped; logs to stderr for warn/error). Included in `setup_lib`.
- `src/setup/setup.c` and `src/setup/setup.h` are stale duplicates and are NOT compiled. Edit `src/setup.c`/`src/setup.h`.
- `src/meson.build` conditionally compiles `color.c` (`fs.exists`); add any new `.c` to `setup_sources`/targets manually.
- Tests use the in-repo macro harness `tests/test_harness.h` (no third-party framework). Add a test executable + `test()` in `tests/meson.build`.

## Store format

Lines are CSV: `path,frequency,duration,is_active,started_at`. Numeric fields are parsed from the right, so paths may contain commas. Legacy 4-field lines (`path,freq,duration,is_active`) and CRLF are accepted. `started_at` is a Unix timestamp set when a directory becomes active.

## Quirks

- `setup_target_path()` finalizes all currently active dirs before activating the target; repeated `cd` to the same active path still increments frequency after finalization.
- `stop_tracking()` / `ting stop` finalizes durations; intended for a shell `EXIT` trap.
- Default store path is absolute and user-specific; do not assume the current user's home.
- `look_up_storage_for_active_path()` is a legacy read-only helper, not used by the CLI.
- Static/global state throughout; not thread-safe or reentrant. Avoid `BUFF_SIZE` (1024) overflow for large stores.
