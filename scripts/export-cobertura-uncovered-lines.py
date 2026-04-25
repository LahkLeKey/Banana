#!/usr/bin/env python3
"""Export uncovered lines from a Cobertura XML report."""

from __future__ import annotations

import argparse
import json
import xml.etree.ElementTree as ET
from pathlib import Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Extract uncovered lines from Cobertura report.")
    parser.add_argument("--report", required=True, help="Path to Cobertura XML report.")
    parser.add_argument("--output-json", required=True, help="Output JSON path.")
    parser.add_argument("--output-text", required=True, help="Output text path.")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    report_path = Path(args.report)
    if not report_path.exists():
        raise FileNotFoundError(f"Cobertura report not found: {report_path}")

    root = ET.parse(report_path).getroot()

    uncovered_by_file: dict[str, list[int]] = {}
    for class_node in root.findall(".//class"):
        filename = class_node.get("filename") or class_node.get("name") or "<unknown>"
        for line_node in class_node.findall("./lines/line"):
            hits_raw = line_node.get("hits", "0")
            line_raw = line_node.get("number", "0")
            try:
                hits = int(hits_raw)
                line_number = int(line_raw)
            except ValueError:
                continue
            if hits == 0 and line_number > 0:
                uncovered_by_file.setdefault(filename, []).append(line_number)

    files_payload = []
    text_lines: list[str] = []
    total = 0
    for file_path in sorted(uncovered_by_file):
        lines = sorted(set(uncovered_by_file[file_path]))
        total += len(lines)
        files_payload.append({"path": file_path, "lines": lines, "count": len(lines)})
        text_lines.extend(f"{file_path}:{line}" for line in lines)

    json_payload = {
        "schema_version": 1,
        "report": str(report_path),
        "total_uncovered_lines": total,
        "files": files_payload,
    }

    output_json = Path(args.output_json)
    output_text = Path(args.output_text)
    output_json.parent.mkdir(parents=True, exist_ok=True)
    output_text.parent.mkdir(parents=True, exist_ok=True)

    output_json.write_text(json.dumps(json_payload, indent=2) + "\n", encoding="utf-8")
    output_text.write_text("\n".join(text_lines) + ("\n" if text_lines else ""), encoding="utf-8")

    print(json.dumps({"files": len(files_payload), "total_uncovered_lines": total}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
