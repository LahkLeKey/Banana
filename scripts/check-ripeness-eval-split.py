#!/usr/bin/env python3
"""247-T006 — Validate that data/ripeness/corpus.json has a realistic eval split.

Exit codes:
  0  All checks pass.
  1  One or more checks fail (prints remediation guidance).
"""

from __future__ import annotations

import argparse
import math
import json
import sys
from collections import Counter
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[1]
CORPUS_PATH = REPO_ROOT / "data" / "ripeness" / "corpus.json"

MIN_EVAL_IDS = 12
MIN_PER_LABEL = 3
MIN_BOUNDARY_RATIO = 0.5

LABELS = ["unripe", "ripe", "overripe", "spoiled"]


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Validate ripeness corpus eval split size, balance, and boundary ratio."
    )
    parser.add_argument(
        "--corpus",
        default=str(CORPUS_PATH),
        help="Path to ripeness corpus JSON (default: data/ripeness/corpus.json).",
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    corpus_path = Path(args.corpus)
    if not corpus_path.exists():
        print(f"[FAIL] Corpus not found: {corpus_path}", file=sys.stderr)
        return 1

    payload = json.loads(corpus_path.read_text(encoding="utf-8"))
    samples = {s["id"]: s for s in payload.get("samples", [])}
    eval_ids: list[str] = payload.get("splits", {}).get("eval_ids", [])

    failures: list[str] = []

    # FR-247-01 — total eval_ids count
    if len(eval_ids) < MIN_EVAL_IDS:
        failures.append(
            f"eval_ids count {len(eval_ids)} < {MIN_EVAL_IDS}. "
            f"Add {MIN_EVAL_IDS - len(eval_ids)} more eval samples "
            "(at least 1 hard-boundary sample per label)."
        )

    # FR-247-01 — per-label count
    label_counts = Counter(
        samples[eid]["label"] for eid in eval_ids if eid in samples
    )
    for label in LABELS:
        count = label_counts.get(label, 0)
        if count < MIN_PER_LABEL:
            failures.append(
                f"eval_ids has {count} '{label}' sample(s), need >= {MIN_PER_LABEL}. "
                f"Add {MIN_PER_LABEL - count} more '{label}' eval sample(s)."
            )

    # FR-247-02 — boundary ratio
    boundary_count = sum(
        1
        for eid in eval_ids
        if eid in samples and "boundary" in samples[eid].get("source", "").lower()
    )
    boundary_ratio = boundary_count / len(eval_ids) if eval_ids else 0.0
    if boundary_ratio < MIN_BOUNDARY_RATIO:
        required = math.ceil(MIN_BOUNDARY_RATIO * len(eval_ids))
        needed = max(0, required - boundary_count)
        failures.append(
            f"boundary_ratio {boundary_ratio:.2f} < {MIN_BOUNDARY_RATIO}. "
            f"At least {needed} more eval samples need source containing 'boundary'."
        )

    if failures:
        print("check-ripeness-eval-split: FAIL")
        for msg in failures:
            print(f"  - {msg}")
        return 1

    print(
        f"check-ripeness-eval-split: PASS  "
        f"eval_ids={len(eval_ids)}  "
        f"per_label={dict(label_counts)}  "
        f"boundary_ratio={boundary_ratio:.2f}"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())