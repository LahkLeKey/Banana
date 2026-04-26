#!/usr/bin/env python3
"""
019 Validation Lane Parity Lint.

Verify that every executable command declared in a feature's
`spec.md` Validation lane and Success Criteria sections is referenced
by at least one task in `tasks.md`.

Usage:
    python scripts/validate-spec-tasks-parity.py <feature-dir>
    python scripts/validate-spec-tasks-parity.py --all

Exit codes:
    0  -- all validators present in tasks.md
    1  -- drift detected (missing validators reported on stderr)
    2  -- usage / IO error
"""
from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
SPECS_DIR = REPO_ROOT / ".specify" / "specs"

# Recognized executable command leaders. Anything that begins with one
# of these tokens (after stripping leading shell noise) is treated as
# a validator command.
COMMAND_LEADERS = {
    "bash",
    "bun",
    "cmake",
    "ctest",
    "dotnet",
    "make",
    "node",
    "npm",
    "pnpm",
    "pytest",
    "python",
    "scripts/",
    "sh",
}

VALIDATION_HEADERS = (
    "Validation lane",
    "Validation Lane",
    "Success Criteria",
    "Success criteria",
)


@dataclass
class Validator:
    """A single executable command extracted from spec.md."""

    raw: str
    tokens: tuple[str, ...]
    source_section: str

    @property
    def signature_tokens(self) -> tuple[str, ...]:
        """Tokens used for matching against tasks.md (flags stripped)."""
        return tuple(t for t in self.tokens if not t.startswith("-"))


def _section_body(text: str, header: str) -> str | None:
    pattern = rf"^#+\s*{re.escape(header)}\s*$(.*?)(?=^#+\s|\Z)"
    m = re.search(pattern, text, re.M | re.S)
    return m.group(1) if m else None


def _iter_command_lines(body: str) -> list[str]:
    """Yield candidate command lines from a section body."""
    lines: list[str] = []
    in_fence = False
    for raw_line in body.splitlines():
        stripped = raw_line.strip()
        if stripped.startswith("```"):
            in_fence = not in_fence
            continue
        if in_fence:
            if stripped and not stripped.startswith("#"):
                lines.append(stripped)
            continue
        # Inline backticked commands inside Success Criteria bullets.
        for m in re.finditer(r"`([^`]+)`", raw_line):
            inner = m.group(1).strip()
            if _looks_like_command(inner):
                lines.append(inner)
    return lines


def _looks_like_command(text: str) -> bool:
    if not text:
        return False
    head = text.split(maxsplit=1)[0]
    if head in COMMAND_LEADERS:
        return True
    return any(head.startswith(prefix) for prefix in COMMAND_LEADERS if prefix.endswith("/"))


def _split_chained(line: str) -> list[str]:
    """Split a line on `&&` so each command is matched independently."""
    parts = [p.strip() for p in re.split(r"&&|;", line) if p.strip()]
    return parts or [line.strip()]


def _tokenize(cmd: str) -> tuple[str, ...]:
    # Strip trailing comments.
    cmd = re.sub(r"\s+#.*$", "", cmd)
    return tuple(cmd.split())


def extract_validators(spec_text: str) -> list[Validator]:
    out: list[Validator] = []
    for header in VALIDATION_HEADERS:
        body = _section_body(spec_text, header)
        if body is None:
            continue
        for line in _iter_command_lines(body):
            for piece in _split_chained(line):
                if not _looks_like_command(piece):
                    continue
                toks = _tokenize(piece)
                if not toks:
                    continue
                out.append(Validator(raw=piece, tokens=toks, source_section=header))
    return out


def matches_in_tasks(validator: Validator, tasks_text: str) -> bool:
    """A validator is present if a >=2-token signature appears in tasks_text.

    The leader plus at least one signature token must appear together
    on a single task line. Flags (-x, --foo) are ignored for matching
    so that a task can reference `dotnet test` even if the spec
    declared `dotnet test --collect:"XPlat Code Coverage"`.

    Special case: if the validator's discriminator token is a script
    path (contains `/` AND ends with `.sh`/`.py`), the path alone is
    sufficient evidence that the task references the script -- the
    leader (e.g. `bash`, `python`) is often omitted in task prose.
    """
    sig = validator.signature_tokens
    if not sig:
        return False
    leader = sig[0]
    distinctive = _most_distinctive(sig[1:]) if len(sig) > 1 else None
    script_path = _script_path_token(sig)
    for raw_line in tasks_text.splitlines():
        if script_path is not None and script_path in raw_line:
            return True
        if leader not in raw_line:
            continue
        if distinctive is None or distinctive in raw_line:
            return True
    return False


def _script_path_token(tokens: tuple[str, ...]) -> str | None:
    for t in tokens:
        if "/" in t and (t.endswith(".sh") or t.endswith(".py")):
            return t
    return None


def _most_distinctive(tokens: tuple[str, ...]) -> str | None:
    """Pick the most script-path-like or longest token as the discriminator."""
    if not tokens:
        return None
    # Prefer tokens that look like script paths or filenames.
    for t in tokens:
        if "/" in t or t.endswith(".sh") or t.endswith(".py"):
            return t
    # Fall back to longest token.
    return max(tokens, key=len)


def check_feature(feature_dir: Path) -> list[Validator]:
    """Return the list of validators that are MISSING from tasks.md."""
    spec = feature_dir / "spec.md"
    tasks = feature_dir / "tasks.md"
    if not spec.exists() or not tasks.exists():
        return []
    spec_text = spec.read_text(encoding="utf-8")
    tasks_text = tasks.read_text(encoding="utf-8")
    validators = extract_validators(spec_text)
    return [v for v in validators if not matches_in_tasks(v, tasks_text)]


def _is_spike(feature_dir: Path) -> bool:
    return feature_dir.name.endswith("-spike")


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("feature_dir", nargs="?", help="Path to a feature directory under .specify/specs/")
    parser.add_argument("--all", action="store_true", help="Check every feature directory")
    parser.add_argument("--include-spikes", action="store_true", help="Also check -spike directories")
    args = parser.parse_args(argv)

    if args.all:
        targets = sorted(p for p in SPECS_DIR.iterdir() if p.is_dir())
    elif args.feature_dir:
        targets = [Path(args.feature_dir).resolve()]
    else:
        parser.print_usage(sys.stderr)
        return 2

    drift_found = False
    for target in targets:
        if not args.include_spikes and _is_spike(target):
            continue
        missing = check_feature(target)
        if missing:
            drift_found = True
            print(f"DRIFT in {target.relative_to(REPO_ROOT) if target.is_relative_to(REPO_ROOT) else target}:", file=sys.stderr)
            for v in missing:
                print(f"  - [{v.source_section}] {v.raw}", file=sys.stderr)
        else:
            print(f"OK    {target.name}")

    return 1 if drift_found else 0


if __name__ == "__main__":
    sys.exit(main())
