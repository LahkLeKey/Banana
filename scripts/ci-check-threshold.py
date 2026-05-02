#!/usr/bin/env python3
"""Check whether a coverage percentage meets a threshold.

Usage: python3 ci-check-threshold.py <percent> [<threshold>]
Prints "1" if percent >= threshold (default 80.0), "0" otherwise.
"""

import sys

percent = float(sys.argv[1])
threshold = float(sys.argv[2]) if len(sys.argv) > 2 else 80.0
print("1" if percent >= threshold else "0")
