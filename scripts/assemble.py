#!/usr/bin/env python3

"""
Generate the monolithic doctest.h header file.

This script scans for any .h and .cpp files in the parts/ directory,
(topologically) sorts them by #include order, then emits them into
the final doctest.h header.
"""

# /// script
# requires-python = ">=3.6"
# ///


import re
import string
import sys
from itertools import chain
from pathlib import Path
from textwrap import dedent


TEMPLATE = string.Template(
    dedent(
        """\
  // ============================================================= lgtm [cpp/missing-header-guard]
  // == DO NOT MODIFY THIS FILE BY HAND - IT IS AUTO GENERATED! ==
  // =============================================================
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

  $headers

  #endif // DOCTEST_LIBRARY_INCLUDED

  #if defined(DOCTEST_CONFIG_IMPLEMENT) && !defined(DOCTEST_LIBRARY_IMPLEMENTATION)

  DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wunused-macros")
  #define DOCTEST_LIBRARY_IMPLEMENTATION
  DOCTEST_CLANG_SUPPRESS_WARNING_POP

  $sources

  #endif // defined(DOCTEST_CONFIG_IMPLEMENT) && !defined(DOCTEST_LIBRARY_IMPLEMENTATION)
"""
    )
)


def main(args):
    """Script entry-point."""

    if len(args) != 1:
        print("Usage: scripts/assemble.py", file=sys.stderr)
        sys.exit(1)

    script = Path(__file__).resolve()
    root = script.parent.parent

    public_dir  = root / "doctest" / "parts" / "public"
    private_dir = root / "doctest" / "parts" / "private"
    output      = root / "doctest" / "doctest.h"

    public_headers  = sorted(set(public_dir.rglob("*.h")))
    private_headers = sorted(set(private_dir.rglob("*.h")))
    private_sources = sorted(set(private_dir.rglob("*.cpp")))

    def extract_header(line):
        """
        Extract a header file name from a line of C code.

        Assuming the input looks something like:

          ```c
          #include "foo.h"
          #include <bar.h>
          ```

        This function will return "foo.h" and "bar.h" respectively
        """

        matches = re.findall(r'#include\s*["<]([^">]+)[">]', line)
        if len(matches) == 0:
            return None
        if len(matches) == 1:
            return matches[0]

        reason = f"'{line}' has multiple includes"
        raise RuntimeError(reason)

    def process_file(file, visited, headers):
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
        content = file.read_text(encoding="utf-8")

        for line in content.splitlines(keepends=False):
            header = extract_header(line)
            if (header is not None) and ((root / header) in headers):
                yield from process_file(root / header, visited=visited, headers=headers)
            else:
                yield line

    visited     = set()
    result = TEMPLATE.substitute(
        headers="\n".join(
            chain.from_iterable(
                process_file(
                    file, visited=visited, headers=public_headers
                )
                for file in public_headers
            )
        ),
        sources="\n".join(
            chain.from_iterable(
                process_file(
                    file, visited=visited, headers=public_headers + private_headers
                )
                for file in private_sources
            )
        ),
    )

    with open(output, "w", encoding="utf-8", newline="\n") as out:
        out.write(result)
    sys.exit(0)


if __name__ == "__main__":
    main(sys.argv)
