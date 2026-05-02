#!/usr/bin/env python3
"""Extract line coverage percentage from a Cobertura-style XML coverage report.

Usage: python3 ci-extract-xml-coverage.py <coverage.xml>
Prints a float percentage like "87.50" to stdout.
"""

import sys
import xml.etree.ElementTree as ET

tree = ET.parse(sys.argv[1])
root = tree.getroot()
line_rate = float(root.attrib.get("line-rate", "0"))
print(f"{line_rate * 100:.2f}")
