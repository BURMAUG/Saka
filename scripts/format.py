#!/usr/bin/env python3
"""Format the C sources.

Runs clang-format, then puts exactly two blank lines before every function
definition, which clang-format cannot do on its own.

    scripts/format.py            format every C file in the repository
    scripts/format.py --check    change nothing; exit 1 if a file would change
    scripts/format.py FILE...    format only the given files
"""

import fnmatch
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
BLANK_LINES = 2

# A function directly after one of these keeps whatever spacing it has.
CONDITIONALS = ("#if", "#else", "#elif")

# The line above a function name when the return type is on its own line.
RETURN_TYPE = re.compile(r"[A-Za-z_][\w \t*]*")


def source_files():
    listing = subprocess.run(
        ["git", "ls-files", "--cached", "--others", "--exclude-standard", "--", "*.c", "*.h"],
        cwd=ROOT,
        capture_output=True,
        text=True,
    )
    if listing.returncode != 0:
        sys.exit("format.py: not a git repository; pass the files to format explicitly")

    ignored = []
    ignore_file = ROOT / ".clang-format-ignore"
    if ignore_file.exists():
        for line in ignore_file.read_text().splitlines():
            line = line.strip()
            if line and not line.startswith("#"):
                ignored.append(line)

    files = []
    for name in listing.stdout.splitlines():
        if any(fnmatch.fnmatch(name, pattern) for pattern in ignored):
            continue
        if (ROOT / name).is_file():
            files.append(ROOT / name)
    return files


def comment_start(lines, start):
    """Index of the first line of the comment block sitting directly above lines[start]."""
    k = start - 1
    while k >= 0:
        stripped = lines[k].strip()
        if stripped.startswith("//"):
            k -= 1
        elif stripped.endswith("*/"):
            opening = k
            while opening >= 0 and "/*" not in lines[opening]:
                opening -= 1
            if opening < 0 or not lines[opening].strip().startswith("/*"):
                break
            k = opening - 1
        else:
            break
    return k + 1


def function_starts(lines):
    """Indexes of the first line of each function definition, comments above it included.

    Relies on the clang-format style: the opening brace of a function body is
    alone at column 0, right after a declarator that ends with ')'.
    """
    starts = set()
    for i, line in enumerate(lines):
        if line != "{" or i == 0 or not lines[i - 1].rstrip().endswith(")"):
            continue
        first = i - 1
        while first > 0 and lines[first][:1] in (" ", "\t"):
            first -= 1
        if first > 0 and RETURN_TYPE.fullmatch(lines[first - 1]):
            first -= 1
        starts.add(comment_start(lines, first))
    return starts


def space_functions(text):
    lines = text.split("\n")
    starts = function_starts(lines)
    out = []
    for i, line in enumerate(lines):
        if i in starts:
            blanks = 0
            while out and out[-1].strip() == "":
                out.pop()
                blanks += 1
            if not out or out[-1].lstrip().startswith(CONDITIONALS):
                out += [""] * blanks
            else:
                out += [""] * BLANK_LINES
        out.append(line)
    return "\n".join(out)


def formatted(path, original):
    result = subprocess.run(
        ["clang-format", str(path)], cwd=ROOT, capture_output=True, text=True, encoding="utf-8"
    )
    if result.returncode != 0 or (original.strip() and not result.stdout.strip()):
        sys.exit(f"format.py: clang-format failed on {path}\n{result.stderr}")
    return space_functions(result.stdout)


def main():
    if shutil.which("clang-format") is None:
        sys.exit("format.py: clang-format is not installed")

    args = sys.argv[1:]
    check = "--check" in args
    files = [Path(arg).resolve() for arg in args if arg != "--check"] or source_files()

    changed = []
    for path in files:
        original = path.read_text(encoding="utf-8")
        result = formatted(path, original)
        if result == original:
            continue
        changed.append(path)
        if not check:
            path.write_text(result, encoding="utf-8")

    for path in changed:
        verb = "would reformat" if check else "reformatted"
        print(f"{verb} {os.path.relpath(path, ROOT)}")
    sys.exit(1 if check and changed else 0)


if __name__ == "__main__":
    main()
