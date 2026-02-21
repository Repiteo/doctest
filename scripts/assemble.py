#!/usr/bin/env python3

"""
Generate the monolithic doctest.h header file.

This script scans for any .h and .cpp files in the parts/ directory,
(topologically) sorts them by #include order, then emits them into
the final doctest.h header.
"""

from __future__ import annotations

import sys
from itertools import chain

if sys.version_info <= (3, 6):
    raise RuntimeError("Python 3.6 or later required.")

import re
import string
from collections.abc import Generator
from pathlib import Path
from typing import NoReturn

ROOT        = Path(__file__).resolve().parent.parent
OUTPUT      = ROOT / "doctest" / "doctest.h"
PUBLIC_DIR  = ROOT / "doctest" / "parts" / "public"
PRIVATE_DIR = ROOT / "doctest" / "parts" / "private"

PUBLIC_HEADERS  = sorted(PUBLIC_DIR.rglob("*.h"))
PRIVATE_HEADERS = sorted(PRIVATE_DIR.rglob("*.h"))
PRIVATE_SOURCES = sorted(PRIVATE_DIR.rglob("*.cpp"))

RE_HEADER_GUARD = re.compile(r"^#(?:ifndef|define|endif) (?:// )?([A-Z0-9_]+)")
RE_INCLUDE      = re.compile(r'^#include ["<]([^">]+)[">]')
RE_GROUPS       = re.compile(
    r"^(?P<guardbegin>#ifndef.*\n#define.*)?"
    + r"\s*"
    + r'(?P<includes>(?:#include ["<][^">]+[">].*\n)*)?'
    + r"\s*"
    + r"(?P<content>(?:.*\n)*?)"
    + r"\s*"
    + r"(?P<guardend>#endif.*)?"
    + r"\s*$",
)

TEMPLATE = string.Template("""\
// =================================================================================================
// == DO NOT MODIFY THIS FILE BY HAND - IT IS AUTO GENERATED! ======================================
// =================================================================================================
//
// doctest.h - the lightest feature-rich C++ single-header testing framework for unit tests and TDD
//
// Copyright (c) 2016-2023 Viktor Kirilov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
// The documentation can be found at the library's page:
// https://github.com/doctest/doctest/blob/master/doc/markdown/readme.md
//
// =================================================================================================
// =================================================================================================
// =================================================================================================
//
// The library is heavily influenced by Catch - https://github.com/catchorg/Catch2
// which uses the Boost Software License - Version 1.0
// see here - https://github.com/catchorg/Catch2/blob/master/LICENSE.txt
//
// The concept of subcases (sections in Catch) and expression decomposition are from there.
// Some parts of the code are taken directly:
// - stringification - the detection of "ostream& operator<<(ostream&, const T&)" and StringMaker<>
// - the Approx() helper class for floating point comparison
// - colors in the console
// - breaking into a debugger
// - signal / SEH handling
// - timer
// - XmlWriter class - thanks to Phil Nash for allowing the direct reuse (AKA copy/paste)
//
// The expression decomposing templates are taken from lest - https://github.com/martinmoene/lest
// which uses the Boost Software License - Version 1.0
// see here - https://github.com/martinmoene/lest/blob/master/LICENSE.txt
//
// =================================================================================================
// =================================================================================================
// =================================================================================================

#ifndef DOCTEST_LIBRARY_INCLUDED
#define DOCTEST_LIBRARY_INCLUDED

$public_headers

#endif // DOCTEST_LIBRARY_INCLUDED

#if defined(DOCTEST_CONFIG_IMPLEMENT) && !defined(DOCTEST_LIBRARY_IMPLEMENTATION)

DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wunused-macros")
#define DOCTEST_LIBRARY_IMPLEMENTATION
DOCTEST_CLANG_SUPPRESS_WARNING_POP

$private_headers

$private_sources

#endif // defined(DOCTEST_CONFIG_IMPLEMENT) && !defined(DOCTEST_LIBRARY_IMPLEMENTATION)
""")

IDENTIFIER = string.Template("""\
// =================================================================================================
// == $full_path
// =================================================================================================
""")


def process_file(file: Path, visited: set[Path]) -> Generator[str, None, None]:
    """
    Process a file, yielding lines of code with #include's and header guards scrubbed.

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
    groups = RE_GROUPS.match(file.read_text(encoding="utf-8", newline="\n")).groupdict()

    if file.suffix == ".h":
        assert groups["guardbegin"] is not None and groups["guardend"] is not None
        guards = [
            RE_HEADER_GUARD.match(guard).group(1)
            for guard in groups["guardbegin"].splitlines() + [groups["guardend"]]
        ]
        assert len(guards) == 3
        assert guards[0] == guards[1] and guards[0] == guards[2]
    else:
        assert not groups["guardbegin"] and not groups["guardend"]

    for include in groups.get("includes", "").splitlines():
        include_path = Path(RE_INCLUDE.match(include).group(1)).resolve()
        yield from process_file(include_path, visited)

    full_path = (file.relative_to(ROOT).as_posix() + " ").ljust(94, "=")
    yield IDENTIFIER.substitute(full_path=full_path)

    for line in groups["content"].split("\n"):
        yield line


def process_files(files: list[Path], visited: set[Path]):
    """Utility linker for processing files."""
    return "\n".join(chain.from_iterable(process_file(file, visited) for file in files))


def main(args) -> NoReturn:
    """Script entry-point."""

    if len(args) != 1:
        print("Usage: scripts/assemble.py", file=sys.stderr)
        sys.exit(1)

    visited = set()

    result = TEMPLATE.substitute(
        public_headers=process_files(PUBLIC_HEADERS, visited),
        private_headers=process_files(PRIVATE_HEADERS, visited),
        private_sources=process_files(PRIVATE_SOURCES, visited),
    )

    with open(OUTPUT, "w", encoding="utf-8", newline="\n") as out:
        out.write(result)

    sys.exit(0)


if __name__ == "__main__":
    main(sys.argv)
