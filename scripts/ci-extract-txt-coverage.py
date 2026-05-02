#!/usr/bin/env python3
"""Extract line coverage percentage from a dotnet coverlet Summary.txt report.

Usage: python3 ci-extract-txt-coverage.py <Summary.txt>
Prints the first percentage value found (e.g. "87.50") or empty string if none.
"""

import re
import sys

text = open(sys.argv[1], encoding="utf-8").read()
matches = re.findall(r"(\d+(?:\.\d+)?)\s*%", text)
print(matches[0] if matches else "")
