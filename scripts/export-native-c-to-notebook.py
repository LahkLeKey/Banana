#!/usr/bin/env python3
"""Generate a Jupyter notebook artifact from native C/H source files."""

from __future__ import annotations

import argparse
import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Iterable


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Export native C sources into a notebook artifact."
    )
    parser.add_argument(
        "--source-root",
        default="src/native",
        help="Workspace-relative source root to scan (default: src/native)",
    )
    parser.add_argument(
        "--output",
        default="notebooks/native-c-catalog.ipynb",
        help="Workspace-relative notebook output path",
    )
    parser.add_argument(
        "--manifest-output",
        default="notebooks/catalog-index.json",
        help="Workspace-relative manifest output path",
    )
    parser.add_argument(
        "--max-lines-per-file",
        type=int,
        default=240,
        help="Maximum lines exported per file before truncation marker",
    )
    return parser.parse_args()


def discover_sources(source_root: Path) -> list[Path]:
    patterns = ("**/*.c", "**/*.h")
    files: list[Path] = []
    for pattern in patterns:
        files.extend(source_root.glob(pattern))
    files = [path for path in files if path.is_file()]
    files.sort(key=lambda path: path.as_posix())
    return files


def make_markdown_cell(lines: Iterable[str]) -> dict:
    return {
        "cell_type": "markdown",
        "metadata": {"language": "markdown"},
        "source": list(lines),
    }


def make_code_cell(lines: Iterable[str]) -> dict:
    return {
        "cell_type": "code",
        "metadata": {"language": "c"},
        "execution_count": None,
        "outputs": [],
        "source": list(lines),
    }


def render_index_cell(files: list[Path], workspace_root: Path) -> dict:
    lines = [
        "# Banana Native C Notebook Catalog\n",
        "Generated for interactive ABI/runtime prototyping.\n",
        "\n",
        "## Included Files\n",
    ]
    for file_path in files:
        relative = file_path.relative_to(workspace_root).as_posix()
        lines.append(f"- `{relative}`\\n")
    return make_markdown_cell(lines)


def render_file_cell(
    file_path: Path,
    workspace_root: Path,
    max_lines_per_file: int,
) -> dict:
    relative = file_path.relative_to(workspace_root).as_posix()
    raw_lines = file_path.read_text(encoding="utf-8", errors="replace").splitlines()

    truncated = len(raw_lines) > max_lines_per_file
    selected = raw_lines[:max_lines_per_file]

    lines = [f"// source: {relative}\\n"]
    lines.extend(f"{line}\\n" for line in selected)
    if truncated:
        lines.append(
            "\n// ... truncated: file exceeded max-lines-per-file; increase --max-lines-per-file to include more.\n"
        )

    return make_code_cell(lines)


def main() -> int:
    args = parse_args()
    workspace_root = Path.cwd()
    source_root = (workspace_root / args.source_root).resolve()
    output_path = (workspace_root / args.output).resolve()
    manifest_path = (workspace_root / args.manifest_output).resolve()

    if not source_root.exists() or not source_root.is_dir():
        raise SystemExit(f"source root not found: {source_root}")

    files = discover_sources(source_root)
    if not files:
        raise SystemExit(f"no .c/.h files found under: {source_root}")

    notebook = {
        "cells": [render_index_cell(files, workspace_root)],
        "metadata": {
            "kernelspec": {
                "display_name": "C (catalog)",
                "language": "c",
                "name": "c-catalog",
            },
            "language_info": {"name": "c"},
        },
        "nbformat": 4,
        "nbformat_minor": 5,
    }

    for file_path in files:
        notebook["cells"].append(
            render_file_cell(
                file_path=file_path,
                workspace_root=workspace_root,
                max_lines_per_file=args.max_lines_per_file,
            )
        )

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(json.dumps(notebook, indent=2), encoding="utf-8")

    manifest = {
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "source_root": source_root.relative_to(workspace_root).as_posix(),
        "notebook_path": output_path.relative_to(workspace_root).as_posix(),
        "file_count": len(files),
        "max_lines_per_file": args.max_lines_per_file,
        "files": [path.relative_to(workspace_root).as_posix() for path in files],
    }
    manifest_path.parent.mkdir(parents=True, exist_ok=True)
    manifest_path.write_text(json.dumps(manifest, indent=2), encoding="utf-8")

    print(f"generated notebook: {output_path}")
    print(f"generated manifest: {manifest_path}")
    print(f"files exported: {len(files)}")
    print(f"max lines per file: {args.max_lines_per_file}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
