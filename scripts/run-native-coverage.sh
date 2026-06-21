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

cd "$ROOT_DIR"

if [[ "$SKIP_BUILD" -eq 0 ]]; then
  echo "[coverage] configuring native coverage build in $BUILD_DIR"
  cmake -S "$ROOT_DIR/src/native" -B "$BUILD_DIR" -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBANANA_ENABLE_COVERAGE=ON

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
coverage_status="ok"

os_name="${OSTYPE:-}"
if [[ "$os_name" == msys* || "$os_name" == cygwin* || "${OS:-}" == "Windows_NT" ]]; then
  if command -v OpenCppCoverage.exe >/dev/null 2>&1; then
    SOURCE_ROOT_W="$(cygpath -w "$ROOT_DIR/src/native" 2>/dev/null || printf '%s' "$ROOT_DIR/src/native")"
    OUTPUT_DIR_W="$(cygpath -w "$OUTPUT_DIR" 2>/dev/null || printf '%s' "$OUTPUT_DIR")"
    echo "[coverage] generating Windows HTML report at $OUTPUT_DIR"
    if ! OpenCppCoverage.exe \
      --working_dir "$ROOT_DIR" \
      --sources "$SOURCE_ROOT_W" \
      --export_type html:"$OUTPUT_DIR_W" \
      --cover_children \
      -- \
      ctest --test-dir "$BUILD_DIR" -C Debug --output-on-failure; then
      coverage_status="warn"
      echo "[coverage] warning: OpenCppCoverage failed; writing fallback inventory summary"
    fi
  else
    coverage_status="warn"
    echo "[coverage] warning: OpenCppCoverage.exe was not found; writing fallback inventory summary"
  fi
else
  if ! command -v gcovr >/dev/null 2>&1; then
    coverage_status="warn"
    echo "[coverage] warning: gcovr is required; writing fallback inventory summary"
  else
    echo "[coverage] generating gcovr report at $OUTPUT_DIR"
    if ! gcovr \
      --root "$ROOT_DIR" \
      --filter "$ROOT_DIR/src/native/" \
      --json "$OUTPUT_DIR/gcovr.json" \
      --html-details "$OUTPUT_DIR/index.html" \
      --txt "$OUTPUT_DIR/coverage-summary.txt" \
      --json-summary "$OUTPUT_DIR/gcovr.json" \
      --print-summary \
      --sort uncovered \
      --exclude '.*tests.*' \
      --exclude '.*third_party.*'; then
      coverage_status="warn"
      echo "[coverage] warning: gcovr failed; writing fallback inventory summary"
    fi
  fi
fi

BANANA_NATIVE_COVERAGE_ROOT_DIR="$ROOT_DIR" BANANA_NATIVE_COVERAGE_REPORT_DIR="$OUTPUT_DIR" BANANA_NATIVE_COVERAGE_STATUS="$coverage_status" python - <<'PY'
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

root_dir = Path(os.environ.get('BANANA_NATIVE_COVERAGE_ROOT_DIR', os.getcwd())).resolve()
source_root = (root_dir / 'src' / 'native').resolve()
if not source_root.exists():
    raise SystemExit(f"native source root does not exist: {source_root}")


def parse_overall_percentage(report_dir: Path):
    summary_txt = report_dir / 'coverage-summary.txt'
    if not summary_txt.exists():
        return None
    for line in summary_txt.read_text(encoding='utf-8', errors='ignore').splitlines():
        if not line.strip().startswith('TOTAL'):
            continue
        match = re.search(r'([0-9.]+)%\s*$', line)
        if match:
            return float(match.group(1))
    return None


def strip_c_comments(text: str) -> str:
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.S)
    text = re.sub(r'//.*?$', '', text, flags=re.M)
    return text


def is_declaration_only_header(path: Path) -> bool:
    if path.suffix.lower() not in {'.h', '.hpp'}:
        return False

    text = path.read_text(encoding='utf-8', errors='ignore')
    text = strip_c_comments(text)

    if re.search(r'\b(static\s+inline|inline)\b', text):
        return False

    if re.search(r'\)\s*\{', text):
        return False

    return True

primary_modules_dir = report_dir / 'Modules'
legacy_modules_dir = report_dir / 'coverage-report-msvc' / 'Modules'
if primary_modules_dir.exists():
  modules_candidates = [primary_modules_dir]
elif legacy_modules_dir.exists():
  modules_candidates = [legacy_modules_dir]
else:
  modules_candidates = []
coverage_by_file = {}

gcovr_json_path = report_dir / 'gcovr.json'
if gcovr_json_path.exists():
    import json
    gcovr_data = json.loads(gcovr_json_path.read_text(encoding='utf-8', errors='ignore') or '{}')
    for entry in gcovr_data.get('files', []):
        file_path = Path(entry.get('file', ''))
        try:
            key = file_path.resolve().relative_to(source_root).as_posix()
        except Exception:
            key = file_path.as_posix()
        lines = (entry.get('summary') or {}).get('lines') or {}
        coverage_by_file[key] = {
            'covered': int(lines.get('covered', 0)),
            'uncovered': int(lines.get('missing', 0)),
            'total': int(lines.get('count', 0)),
        }
else:
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
                key = Path(title).name
                covered = len(re.findall(r'background-color:#dfd', text))
                uncovered = len(re.findall(r'background-color:#fdd', text))
                total = covered + uncovered
                if total <= 0:
                    continue
                coverage_by_file[key] = {
                    'covered': coverage_by_file.get(key, {}).get('covered', 0) + covered,
                    'uncovered': coverage_by_file.get(key, {}).get('uncovered', 0) + uncovered,
                    'total': coverage_by_file.get(key, {}).get('total', 0) + total,
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
declaration_only_count = 0
for path in all_sources:
    rel = path.relative_to(source_root).as_posix()
    entry = coverage_by_file.get(rel) or coverage_by_file.get(path.name)
    actionable = True
    coverage_display = '0.0%'
    if entry is None:
        line_count = sum(1 for _ in path.read_text(encoding='utf-8', errors='ignore').splitlines())
        if is_declaration_only_header(path):
            covered = 0
            uncovered = 0
            status = 'declaration-only'
            observed = False
            actionable = False
            declaration_only_count += 1
        else:
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
        if is_declaration_only_header(path):
            covered = 0
            uncovered = 0
            status = 'declaration-only'
            actionable = False
            declaration_only_count += 1

    if covered + uncovered > 0:
        pct = 100.0 * covered / (covered + uncovered)
        coverage_display = f'{pct:.1f}%'
    else:
        pct = 0.0
        coverage_display = 'n/a' if not actionable else '0.0%'

    if actionable:
        covered_total += covered
        uncovered_total += uncovered

    rows.append((rel, covered, uncovered, covered + uncovered, pct, status, observed, actionable, coverage_display))

rows.sort(key=lambda item: (not item[7], item[2] == 0, item[2], item[0]), reverse=False)

summary_path = report_dir / 'coverage-files-summary.html'
overall_pct = parse_overall_percentage(report_dir)
if overall_pct is None:
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
<p class="meta">This view enumerates every native source file under src/native. Files that were not observed in the current coverage run are shown as <strong>not observed</strong> and are treated as uncovered so the inventory is representative of executable native code paths.</p>
<p class="meta">Declaration-only headers are marked <strong>declaration-only</strong> and excluded from the actionable coverage denominator.</p>
<p class="meta">Overall coverage across the current inventory: <strong>{overall_pct:.1f}%</strong> ({covered_total} covered lines / {covered_total + uncovered_total} total lines).</p>
<p class="meta">Declaration-only headers detected in this run: <strong>{declaration_only_count}</strong>.</p>
<table>
<thead><tr><th>Source file</th><th>Status</th><th>Covered</th><th>Uncovered</th><th>Total</th><th>Coverage</th></tr></thead>
<tbody>
{''.join(
    f'<tr><td><strong>{html.escape(rel)}</strong></td><td>{status}</td><td>{covered}</td><td>{uncovered}</td><td>{total}</td><td class="{cls}">{coverage_display}</td></tr>'
    for rel, covered, uncovered, total, pct, status, observed, actionable, coverage_display in rows
    for cls in ['low' if actionable and pct < 60 else 'good']
)}
</tbody></table>
<p class="meta">Open the detailed OpenCppCoverage report at <a href="index.html">index.html</a>.</p>
</body></html>
'''
summary_path.write_text(summary_html, encoding='utf-8')
summary_md = report_dir / 'coverage-summary.md'
summary_md.write_text(
    f"# Native source inventory coverage\n\n"
    f"- Overall inventory coverage: {overall_pct:.1f}%\n"
    f"- Source inventory summary: artifacts/native/coverage-ci/coverage-files-summary.html\n"
    f"- Detailed report: artifacts/native/coverage-ci/index.html\n",
    encoding='utf-8'
)

print(f"[coverage] wrote source inventory summary to {summary_path}")
print(f"[coverage] overall inventory coverage: {overall_pct:.1f}%")
if os.environ.get('BANANA_NATIVE_COVERAGE_STATUS', 'ok') != 'ok':
    print(f"[coverage] warning: coverage tool reported a problem; fallback inventory summary was written")
PY

echo "[coverage] report ready: $OUTPUT_DIR"
if [[ -f "$OUTPUT_DIR/coverage-summary.md" ]]; then
  echo "[coverage] summary"
  cat "$OUTPUT_DIR/coverage-summary.md"
fi
