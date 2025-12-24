#!/usr/bin/env python3

"""
Generate the monolithic doctest.h header file.

This script scans for any .h and .cpp files in the parts/ directory,
(topologically) sorts them by #include order, then emits them into
the final doctest.h header.
"""

from __future__ import annotations

import sys

if sys.version_info <= (3, 6):
    raise RuntimeError("Python 3.6 or later required.")

import re
import string
from itertools import chain
from pathlib import Path
from typing import Generator

ROOT = Path(__file__).resolve().parent.parent
OUTPUT = ROOT / "doctest" / "doctest.h"
DOCTEST_FWD = ROOT / "doctest" / "parts" / "doctest_fwd.h"

PUBLIC_DIR = ROOT / "doctest" / "parts" / "public"
PRIVATE_DIR = ROOT / "doctest" / "parts" / "private"

PUBLIC_HEADERS = sorted(PUBLIC_DIR.rglob("*.h"))
PRIVATE_HEADERS = sorted(PRIVATE_DIR.rglob("*.h"))
PRIVATE_SOURCES = sorted(PRIVATE_DIR.rglob("*.cpp"))

RE_HEADER = re.compile(r'^\s*#include\s*["<]([^">]+)[">]')

TEMPLATE = string.Template("""\
// =============================================================
// == DO NOT MODIFY THIS FILE BY HAND - IT IS AUTO GENERATED! ==
// =============================================================
$headers

#if defined(DOCTEST_CONFIG_IMPLEMENT)

DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wunused-macros")
#ifndef DOCTEST_LIBRARY_IMPLEMENTATION
#define DOCTEST_LIBRARY_IMPLEMENTATION
DOCTEST_CLANG_SUPPRESS_WARNING_POP

DOCTEST_SUPPRESS_PRIVATE_WARNINGS_PUSH

$sources

DOCTEST_SUPPRESS_PRIVATE_WARNINGS_POP

#endif // DOCTEST_LIBRARY_IMPLEMENTATION

#endif // DOCTEST_CONFIG_IMPLEMENT
""")

FILE_IDENTIFIER = string.Template("""\
// =================================================================================================
// == $filename
// =================================================================================================
""")
FILE_IDENTIFIER_MARGIN = 94


def generate_identifier(path: Path) -> str:
    filename = path.relative_to(ROOT).as_posix()
    filename = (filename + " ").ljust(FILE_IDENTIFIER_MARGIN, "=")
    return FILE_IDENTIFIER.substitute(filename=filename)


def extract_header(line: str) -> str:
    """
    Extract a header file name from a line of C code.

    Assuming the input looks something like:

        ```c
        #include "foo.h"
        #include <bar.h>
        ```

    This function will return "foo.h" and "bar.h" respectively
    """

    # match = RE_HEADER.match(line)
    # return match.string if match else ""

    matches = RE_HEADER.findall(line)
    if len(matches) == 0:
        return ""
    if len(matches) == 1:
        return matches[0]

    reason = f"'{line}' has multiple includes"
    raise RuntimeError(reason)


def process_file(file: Path, visited: set[Path], headers: list[Path]) -> Generator[str]:
    """
    Process a file, yielding lines of code with #include's scrubbed.

    Assuming the file represents a C source file, iterates over each line,
    yielding the contents. If the line is an #include which has NOT
    yet been seen (as indicated by the `visitor` set), then the
    contents of THAT file is recursively run through this same method.

    If the file HAS been visited, then it is ignored and the line is not yielded.

    The effect of this is a fusion of:
        1. A topological sort by-header, and
        2. Inlining header content, and
        3. Scrubbing #include's to other doctest files
    """

    if file in visited:
        return

    visited.add(file)
    content = file.read_text(encoding="utf-8").splitlines()
    trim_begin: int = -1
    trim_end: int = -1

    for index, line in enumerate(content):
        if not line or line in [
            "DOCTEST_SUPPRESS_COMMON_WARNINGS_PUSH",
            "DOCTEST_SUPPRESS_PUBLIC_WARNINGS_PUSH",
            "DOCTEST_SUPPRESS_PRIVATE_WARNINGS_PUSH",
            "#pragma once",
        ]:
            continue
        header = extract_header(line)
        if header and ((ROOT / header) in headers):
            yield from process_file(ROOT / header, visited=visited, headers=headers)
            continue
        trim_begin = index
        break

    for index, line in reversed(list(enumerate(content))):
        if not line or line in [
            "DOCTEST_SUPPRESS_COMMON_WARNINGS_POP",
            "DOCTEST_SUPPRESS_PUBLIC_WARNINGS_POP",
            "DOCTEST_SUPPRESS_PRIVATE_WARNINGS_POP",
        ]:
            continue
        trim_end = index + 1
        break

    if file != DOCTEST_FWD:
        yield generate_identifier(file)

    for line in content[trim_begin:trim_end] + [""]:
        header = extract_header(line)
        if header and ((ROOT / header) in headers):
            yield from process_file(ROOT / header, visited=visited, headers=headers)
        else:
            yield line


def main(args):
    """Script entry-point."""

    if len(args) != 1:
        print("Usage: scripts/assemble.py", file=sys.stderr)
        sys.exit(1)

    visited = set()
    result = TEMPLATE.substitute(
        headers="\n".join(
            process_file(DOCTEST_FWD, visited=visited, headers=PUBLIC_HEADERS)
        ),
        sources="\n".join(
            chain.from_iterable(
                process_file(
                    file,
                    visited=visited,
                    headers=PUBLIC_HEADERS + PRIVATE_HEADERS + [DOCTEST_FWD],
                )
                for file in PRIVATE_SOURCES
            )
        ),
    )

    with open(OUTPUT, "w", encoding="utf-8", newline="\n") as out:
        out.write(result)
    sys.exit(0)


if __name__ == "__main__":
    main(sys.argv)
