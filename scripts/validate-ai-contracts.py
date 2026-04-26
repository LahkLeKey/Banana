#!/usr/bin/env python3
"""Compatibility wrapper that forwards to the canonical .specify validator."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TARGET = ROOT / ".specify" / "scripts" / "validate-ai-contracts.py"


def main() -> int:
    if not TARGET.exists():
        print(f"Validator not found: {TARGET}", file=sys.stderr)
        return 1
    command = [sys.executable, str(TARGET), *sys.argv[1:]]
    return subprocess.call(command)


if __name__ == "__main__":
    raise SystemExit(main())
