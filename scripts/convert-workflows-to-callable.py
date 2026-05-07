#!/usr/bin/env python3
"""Convert standalone GitHub Actions workflows to reusable workflow_call workflows.

Rewrites the top-level `on:` block to `on: { workflow_call: {} }` while
preserving everything else verbatim. Skips files that already contain
`workflow_call:` at the top level.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path


def rewrite(path: Path) -> bool:
    text = path.read_text(encoding="utf-8")
    lines = text.splitlines(keepends=True)

    on_start = None
    for i, line in enumerate(lines):
        if re.match(r"^on\s*:", line):
            on_start = i
            break

    if on_start is None:
        print(f"  SKIP (no 'on:' block): {path.name}")
        return False

    # Find end of on block: next top-level key (column 0, not blank/comment)
    on_end = len(lines)
    for j in range(on_start + 1, len(lines)):
        line = lines[j]
        stripped = line.rstrip("\n")
        if not stripped or stripped.lstrip().startswith("#"):
            continue
        if re.match(r"^[A-Za-z_]", line):  # new top-level key
            on_end = j
            break

    on_block = "".join(lines[on_start:on_end])
    if "workflow_call:" in on_block:
        print(f"  SKIP (already callable): {path.name}")
        return False

    new_on = "on:\n  workflow_call: {}\n"
    new_text = "".join(lines[:on_start]) + new_on + "".join(lines[on_end:])
    path.write_text(new_text, encoding="utf-8")
    print(f"  REWROTE: {path.name}")
    return True


def main() -> int:
    targets = [Path(p) for p in sys.argv[1:]]
    if not targets:
        print(
            "Usage: convert-workflows-to-callable.py <file> [<file>...]",
            file=sys.stderr,
        )
        return 2
    changed = 0
    for t in targets:
        if not t.exists():
            print(f"  MISSING: {t}")
            continue
        if rewrite(t):
            changed += 1
    print(f"Rewrote {changed} workflow(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
