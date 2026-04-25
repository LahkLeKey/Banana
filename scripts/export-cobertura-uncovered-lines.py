#!/usr/bin/env python3
"""Export uncovered line inventory from a Cobertura XML report."""

from __future__ import annotations

import argparse
import json
import pathlib
import sys
import xml.etree.ElementTree as ET


def _tag_name(tag: str) -> str:
    if "}" in tag:
        return tag.rsplit("}", 1)[1]
    return tag


def _iter_elements(root: ET.Element, name: str):
    for element in root.iter():
        if _tag_name(element.tag) == name:
            yield element


def _parse_hits(raw_hits: str | None) -> int:
    if raw_hits is None:
        return 0
    try:
        return int(float(raw_hits))
    except ValueError:
        return 0


def _parse_line_number(raw_number: str | None) -> int | None:
    if raw_number is None:
        return None
    try:
        return int(raw_number)
    except ValueError:
        return None


def build_inventory(report_path: pathlib.Path) -> dict:
    tree = ET.parse(report_path)
    root = tree.getroot()

    uncovered_by_file: dict[str, set[int]] = {}

    for class_element in _iter_elements(root, "class"):
        filename = (class_element.attrib.get("filename") or "").replace("\\", "/").strip()
        if not filename:
            continue

        for line_element in _iter_elements(class_element, "line"):
            line_number = _parse_line_number(line_element.attrib.get("number"))
            if line_number is None:
                continue

            hits = _parse_hits(line_element.attrib.get("hits"))
            if hits > 0:
                continue

            uncovered_by_file.setdefault(filename, set()).add(line_number)

    files = []
    total_uncovered_lines = 0
    for file_path, lines in sorted(uncovered_by_file.items()):
        ordered_lines = sorted(lines)
        uncovered_count = len(ordered_lines)
        total_uncovered_lines += uncovered_count
        files.append(
            {
                "file": file_path,
                "uncoveredLineCount": uncovered_count,
                "lines": ordered_lines,
            }
        )

    files.sort(key=lambda item: (-item["uncoveredLineCount"], item["file"]))

    return {
        "reportPath": str(report_path).replace("\\", "/"),
        "uncoveredFileCount": len(files),
        "uncoveredLineCount": total_uncovered_lines,
        "files": files,
    }


def write_text_report(payload: dict, output_path: pathlib.Path) -> None:
    lines: list[str] = []
    lines.append(f"Uncovered file count: {payload['uncoveredFileCount']}")
    lines.append(f"Uncovered line count: {payload['uncoveredLineCount']}")
    lines.append("")

    for item in payload["files"]:
        numbers = ",".join(str(number) for number in item["lines"])
        lines.append(f"{item['file']} ({item['uncoveredLineCount']}): {numbers}")

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--report", required=True, help="Path to Cobertura XML report.")
    parser.add_argument("--output-json", required=True, help="Path to write uncovered-line JSON payload.")
    parser.add_argument("--output-text", required=True, help="Path to write uncovered-line text summary.")
    args = parser.parse_args()

    report_path = pathlib.Path(args.report)
    if not report_path.exists():
        print(f"Cobertura report not found: {report_path}", file=sys.stderr)
        return 1

    payload = build_inventory(report_path)

    output_json = pathlib.Path(args.output_json)
    output_json.parent.mkdir(parents=True, exist_ok=True)
    output_json.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    write_text_report(payload, pathlib.Path(args.output_text))

    print(
        "Generated uncovered-line inventory: "
        f"files={payload['uncoveredFileCount']} lines={payload['uncoveredLineCount']}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
