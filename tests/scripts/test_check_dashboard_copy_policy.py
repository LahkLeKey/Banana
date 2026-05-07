"""Tests for check-dashboard-copy-policy.ts (Spec 861).

Covers:
- Compliant sources pass with exit 0
- Restricted term in user-visible copy fails with exit 1
- Restricted term in an import statement is skipped
- Restricted term in a comment line is skipped
- Unknown argument exits with code 2
- Missing terms file exits with code 2
- Bad terms file JSON exits with code 2
- Non-strict mode reports violations but exits 0
"""

from __future__ import annotations

import json
import subprocess
from pathlib import Path

SCRIPT = Path(__file__).parents[2] / "scripts" / "check-dashboard-copy-policy.ts"
TERMS = Path(__file__).parents[2] / "scripts" / "dashboard-copy-policy-terms.json"


def run(*extra_args: str, cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        ["bun", str(SCRIPT), *extra_args],
        capture_output=True,
        text=True,
        cwd=str(cwd) if cwd else None,
    )


def write_tsx(tmp_path: Path, content: str) -> Path:
    p = tmp_path / "TestPage.tsx"
    p.write_text(content, encoding="utf-8")
    return p


def write_terms(tmp_path: Path, terms: list[str]) -> Path:
    p = tmp_path / "terms.json"
    p.write_text(json.dumps(terms), encoding="utf-8")
    return p


# ---------------------------------------------------------------------------
# Happy path
# ---------------------------------------------------------------------------


def test_compliant_source_passes(tmp_path: Path) -> None:
    source = write_tsx(tmp_path, "const label = 'Telemetry Dashboard';")
    terms = write_terms(tmp_path, ["ChatGPT", "OpenAI"])
    result = run("--files", str(source), "--terms", str(terms))
    assert result.returncode == 0
    assert "OK" in result.stdout


def test_default_files_pass_clean_repo(tmp_path: Path) -> None:
    # Run against actual repo files with the real terms list to confirm no violations
    result = run("--terms", str(TERMS))
    assert result.returncode == 0


# ---------------------------------------------------------------------------
# Violation detection
# ---------------------------------------------------------------------------


def test_restricted_term_in_jsx_text_fails(tmp_path: Path) -> None:
    source = write_tsx(
        tmp_path,
        "const msg = 'Powered by ChatGPT technology';",
    )
    terms = write_terms(tmp_path, ["ChatGPT"])
    result = run("--files", str(source), "--terms", str(terms))
    assert result.returncode == 1
    assert "ChatGPT" in result.stderr


def test_multiple_violations_all_reported(tmp_path: Path) -> None:
    source = write_tsx(
        tmp_path,
        "\n".join(
            [
                "const a = 'Contact OpenAI support';",
                "const b = 'Ask Anthropic for help';",
            ]
        ),
    )
    terms = write_terms(tmp_path, ["OpenAI", "Anthropic"])
    result = run("--files", str(source), "--terms", str(terms))
    assert result.returncode == 1
    assert "OpenAI" in result.stderr
    assert "Anthropic" in result.stderr


# ---------------------------------------------------------------------------
# Skipped contexts
# ---------------------------------------------------------------------------


def test_term_in_import_statement_is_skipped(tmp_path: Path) -> None:
    source = write_tsx(tmp_path, "import OpenAI from 'openai-sdk';")
    terms = write_terms(tmp_path, ["OpenAI"])
    result = run("--files", str(source), "--terms", str(terms))
    assert result.returncode == 0, result.stderr


def test_term_in_double_slash_comment_is_skipped(tmp_path: Path) -> None:
    source = write_tsx(tmp_path, "// TODO: remove ChatGPT reference")
    terms = write_terms(tmp_path, ["ChatGPT"])
    result = run("--files", str(source), "--terms", str(terms))
    assert result.returncode == 0, result.stderr


def test_term_in_jsdoc_star_line_is_skipped(tmp_path: Path) -> None:
    source = write_tsx(tmp_path, " * This does not use Claude.")
    terms = write_terms(tmp_path, ["Claude"])
    result = run("--files", str(source), "--terms", str(terms))
    assert result.returncode == 0, result.stderr


# ---------------------------------------------------------------------------
# Non-strict mode
# ---------------------------------------------------------------------------


def test_no_strict_mode_exits_0_with_violations(tmp_path: Path) -> None:
    source = write_tsx(tmp_path, "const label = 'Powered by Gemini';")
    terms = write_terms(tmp_path, ["Gemini"])
    result = run("--files", str(source), "--terms", str(terms), "--no-strict")
    assert result.returncode == 0
    # Violation should still be reported to stderr
    assert "Gemini" in result.stderr


# ---------------------------------------------------------------------------
# Error conditions
# ---------------------------------------------------------------------------


def test_unknown_argument_exits_2(tmp_path: Path) -> None:
    result = run("--invalid-flag")
    assert result.returncode == 2


def test_missing_terms_file_exits_2(tmp_path: Path) -> None:
    source = write_tsx(tmp_path, "const x = 1;")
    result = run("--files", str(source), "--terms", str(tmp_path / "missing.json"))
    assert result.returncode == 2
    assert "not found" in result.stderr


def test_bad_terms_json_exits_2(tmp_path: Path) -> None:
    source = write_tsx(tmp_path, "const x = 1;")
    bad_terms = tmp_path / "bad.json"
    bad_terms.write_text("not json", encoding="utf-8")
    result = run("--files", str(source), "--terms", str(bad_terms))
    assert result.returncode == 2


def test_missing_source_file_exits_2(tmp_path: Path) -> None:
    terms = write_terms(tmp_path, ["ChatGPT"])
    result = run("--files", str(tmp_path / "missing.tsx"), "--terms", str(terms))
    assert result.returncode == 2
    assert "not found" in result.stderr
