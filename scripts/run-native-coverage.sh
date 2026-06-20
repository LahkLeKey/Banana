#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BANANA_NATIVE_BUILD_DIR:-out/native-coverage}"
OUTPUT_DIR="${BANANA_NATIVE_COVERAGE_OUTPUT_DIR:-artifacts/native/coverage}"
SKIP_BUILD=0
SKIP_TESTS=0

usage() {
  cat <<'EOF_USAGE'
Usage:
  bash scripts/run-native-coverage.sh [options]

Options:
  --build-dir <path>      CMake build directory (default: out/native-coverage)
  --output-dir <path>     Coverage report output directory (default: artifacts/native/coverage)
  --skip-build            Reuse the existing build tree without reconfiguring/building
  --skip-tests            Skip ctest execution and only generate the coverage report
  --help                  Show this help

Examples:
  bash scripts/run-native-coverage.sh
  bash scripts/run-native-coverage.sh --build-dir out/native-coverage-ci --output-dir artifacts/native/coverage-ci
EOF_USAGE
}

die() {
  echo "[coverage] error: $*" >&2
  exit 1
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --build-dir)
      [[ $# -ge 2 ]] || die "--build-dir requires a value"
      BUILD_DIR="$2"
      shift 2
      ;;
    --output-dir)
      [[ $# -ge 2 ]] || die "--output-dir requires a value"
      OUTPUT_DIR="$2"
      shift 2
      ;;
    --skip-build)
      SKIP_BUILD=1
      shift
      ;;
    --skip-tests)
      SKIP_TESTS=1
      shift
      ;;
    --help|-h)
      usage
      exit 0
      ;;
    *)
      die "unknown argument '$1'"
      ;;
  esac
done

mkdir -p "$ROOT_DIR"

if [[ "$SKIP_BUILD" -eq 0 ]]; then
  if [[ -f "$BUILD_DIR/CMakeCache.txt" ]]; then
    echo "[coverage] reusing existing build tree at $BUILD_DIR"
  else
    echo "[coverage] configuring native coverage build in $BUILD_DIR"
    cmake -S "$ROOT_DIR/src/native" -B "$BUILD_DIR" -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBANANA_ENABLE_COVERAGE=ON
  fi

  echo "[coverage] building native targets"
  cmake --build "$BUILD_DIR" --parallel
else
  echo "[coverage] reusing existing build tree at $BUILD_DIR"
fi

if [[ "$SKIP_TESTS" -eq 0 ]]; then
  echo "[coverage] running native tests"
  if [[ -f "$BUILD_DIR/CMakeCache.txt" ]]; then
    ctest --test-dir "$BUILD_DIR" -C Debug --output-on-failure
  else
    ctest --test-dir "$BUILD_DIR" --output-on-failure
  fi
else
  echo "[coverage] skipping native tests"
fi

mkdir -p "$OUTPUT_DIR"

if [[ "$OSTYPE" == msys* || "$OSTYPE" == cygwin* || "$OS" == "Windows_NT" ]]; then
  if command -v OpenCppCoverage.exe >/dev/null 2>&1; then
    SOURCE_ROOT_W="$(cygpath -w "$ROOT_DIR/src/native" 2>/dev/null || printf '%s' "$ROOT_DIR/src/native")"
    OUTPUT_DIR_W="$(cygpath -w "$OUTPUT_DIR" 2>/dev/null || printf '%s' "$OUTPUT_DIR")"
    echo "[coverage] generating Windows HTML report at $OUTPUT_DIR"
    OpenCppCoverage.exe \
      --working_dir "$ROOT_DIR" \
      --sources "$SOURCE_ROOT_W" \
      --export_type html:"$OUTPUT_DIR_W" \
      --cover_children \
      -- \
      ctest --test-dir "$BUILD_DIR" -C Debug --output-on-failure
  else
    die "OpenCppCoverage.exe was not found. Install OpenCppCoverage or run this script on a Unix-like environment with gcovr installed."
  fi
else
  if ! command -v gcovr >/dev/null 2>&1; then
    die "gcovr is required. Install it with your package manager (for example: sudo apt-get install -y gcovr lcov)."
  fi

  echo "[coverage] generating gcovr report at $OUTPUT_DIR"
  gcovr \
    --root "$ROOT_DIR" \
    --filter "$ROOT_DIR/src/native/" \
    --html-details "$OUTPUT_DIR/index.html" \
    --txt "$OUTPUT_DIR/coverage-summary.txt" \
    --print-summary \
    --sort uncovered \
    --exclude '.*tests.*' \
    --exclude '.*third_party.*'
fi

BANANA_NATIVE_COVERAGE_REPORT_DIR="$OUTPUT_DIR" python - <<'PY'
import os
import re
import html
from pathlib import Path

report_dir = Path(os.environ.get('BANANA_NATIVE_COVERAGE_REPORT_DIR', ''))
if not report_dir:
    raise SystemExit('BANANA_NATIVE_COVERAGE_REPORT_DIR is empty')
report_dir = report_dir.resolve() if report_dir.exists() else Path(report_dir)
if not report_dir.exists():
    raise SystemExit(f"coverage report directory does not exist: {report_dir}")

source_root = (Path(os.getcwd()) / 'src' / 'native').resolve()
if not source_root.exists():
    raise SystemExit(f"native source root does not exist: {source_root}")

modules_candidates = [report_dir / 'Modules', report_dir / 'coverage-report-msvc' / 'Modules']
coverage_by_file = {}
for modules_dir in modules_candidates:
    if not modules_dir.exists():
        continue
    for module_dir in sorted(modules_dir.iterdir()):
        if not module_dir.is_dir():
            continue
        for html_file in sorted(module_dir.glob('*.html')):
            if html_file.name == 'index.html':
                continue
            text = html_file.read_text(encoding='utf-8', errors='ignore')
            title_match = re.search(r'<title>(.*?)</title>', text, re.I | re.S)
            title = title_match.group(1).strip() if title_match else html_file.stem
            covered = len(re.findall(r'background-color:#dfd', text))
            uncovered = len(re.findall(r'background-color:#fdd', text))
            total = covered + uncovered
            if total <= 0:
                continue
            coverage_by_file[title] = {
                'covered': coverage_by_file.get(title, {}).get('covered', 0) + covered,
                'uncovered': coverage_by_file.get(title, {}).get('uncovered', 0) + uncovered,
                'total': coverage_by_file.get(title, {}).get('total', 0) + total,
            }

rows = []
all_sources = []
for path in sorted(source_root.rglob('*')):
    if not path.is_file():
        continue
    if path.suffix.lower() not in {'.c', '.h', '.cpp', '.hpp'}:
        continue
    if 'third_party' in path.parts or '.git' in path.parts or 'out' in path.parts:
        continue
    all_sources.append(path)

covered_total = 0
uncovered_total = 0
for path in all_sources:
    rel = path.relative_to(source_root).as_posix()
    entry = coverage_by_file.get(path.name)
    if entry is None:
        line_count = sum(1 for _ in path.read_text(encoding='utf-8', errors='ignore').splitlines())
        covered = 0
        uncovered = line_count
        status = 'not observed'
        observed = False
    else:
        line_count = entry['total'] if entry['total'] > 0 else sum(1 for _ in path.read_text(encoding='utf-8', errors='ignore').splitlines())
        covered = entry['covered']
        uncovered = entry['uncovered']
        status = 'partial' if covered and uncovered else ('fully covered' if covered else 'no observed lines')
        observed = True
    if covered + uncovered > 0:
        pct = 100.0 * covered / (covered + uncovered)
    else:
        pct = 0.0
    covered_total += covered
    uncovered_total += uncovered
    rows.append((rel, covered, uncovered, covered + uncovered, pct, status, observed))

rows.sort(key=lambda item: (item[2] == 0, item[2], item[0]), reverse=False)

summary_path = report_dir / 'coverage-files-summary.html'
overall_pct = (100.0 * covered_total / (covered_total + uncovered_total)) if (covered_total + uncovered_total) else 0.0
summary_html = f'''<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8" />
<title>Native source inventory coverage</title>
<style>body{{font-family:Segoe UI,Arial,sans-serif;margin:24px;color:#222;}} table{{border-collapse:collapse;width:100%;max-width:1400px;}} th,td{{border:1px solid #d0d7de;padding:8px 10px;text-align:left;}} th{{background:#f6f8fa;}} tr:nth-child(even){{background:#fafbfc;}} a{{color:#0969da;text-decoration:none;}} .low{{color:#cf222e;font-weight:600;}} .good{{color:#1a7f37;font-weight:600;}} .meta{{color:#57606a;font-size:0.95em;}} .warn{{color:#8250df;font-weight:600;}}</style>
</head>
<body>
<h1>Native source inventory coverage</h1>
<p class="meta">This view enumerates every native source file under src/native. Files that were not observed in the current coverage run are shown as <strong>not observed</strong> and are treated as uncovered so the inventory is representative of the full native codebase.</p>
<p class="meta">Overall coverage across the current inventory: <strong>{overall_pct:.1f}%</strong> ({covered_total} covered lines / {covered_total + uncovered_total} total lines).</p>
<table>
<thead><tr><th>Source file</th><th>Status</th><th>Covered</th><th>Uncovered</th><th>Total</th><th>Coverage</th></tr></thead>
<tbody>
{''.join(
    f'<tr><td><strong>{html.escape(rel)}</strong></td><td>{status}</td><td>{covered}</td><td>{uncovered}</td><td>{total}</td><td class="{cls}">{pct:.1f}%</td></tr>'
    for rel, covered, uncovered, total, pct, status, observed in rows
    for cls in ['low' if pct < 60 else 'good']
)}
</tbody></table>
<p class="meta">Open the detailed OpenCppCoverage report at <a href="index.html">index.html</a>.</p>
</body></html>
'''
summary_path.write_text(summary_html, encoding='utf-8')

print(f"[coverage] wrote source inventory summary to {summary_path}")
print(f"[coverage] overall inventory coverage: {100.0 * covered_total / (covered_total + uncovered_total) if (covered_total + uncovered_total) else 0.0:.1f}%")
PY

echo "[coverage] report ready: $OUTPUT_DIR"
if [[ -f "$OUTPUT_DIR/coverage-summary.txt" ]]; then
  echo "[coverage] summary"
  cat "$OUTPUT_DIR/coverage-summary.txt"
fi
