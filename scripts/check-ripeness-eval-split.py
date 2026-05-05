#!/usr/bin/env python3
"""247-T006 — Validate that data/ripeness/corpus.json has a realistic eval split.

Exit codes:
  0  All checks pass.
  1  One or more checks fail (prints remediation guidance).
"""

from __future__ import annotations

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


def main() -> int:
    if not CORPUS_PATH.exists():
        print(f"[FAIL] Corpus not found: {CORPUS_PATH}", file=sys.stderr)
        return 1

    payload = json.loads(CORPUS_PATH.read_text(encoding="utf-8"))
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
        needed = int(MIN_BOUNDARY_RATIO * len(eval_ids)) - boundary_count + 1
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
