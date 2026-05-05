#!/usr/bin/env python3
"""248-T008 — Score the ripeness challenge set against the trained vocabulary.

Loads artifacts/training/ripeness/local/vocabulary.json and runs each prompt
in data/ripeness/challenge-set.json through the bag-of-words scorer.
Reports per-label recall and flags labels below the PASS threshold.

Exit codes:
  0  All per-label recall values meet the threshold.
  1  One or more labels fall below the threshold (prints remediation guidance).
"""

from __future__ import annotations

import json
import re
import sys
from collections import Counter
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[1]
VOCAB_PATH = REPO_ROOT / "artifacts" / "training" / "ripeness" / "local" / "vocabulary.json"
CHALLENGE_PATH = REPO_ROOT / "data" / "ripeness" / "challenge-set.json"

TOKEN_PATTERN = re.compile(r"[a-z0-9']+")
MIN_TOKEN_LENGTH = 3
PASS_RECALL_THRESHOLD = 0.5  # at least half of challenge prompts per label must be correct


def tokenize(text: str) -> list[str]:
    tokens = []
    for raw in TOKEN_PATTERN.findall(text.lower()):
        token = raw.strip("'")
        if len(token) >= MIN_TOKEN_LENGTH and not token.isdigit():
            tokens.append(token)
    return tokens


def predict(text: str, weights: dict[str, dict[str, float]], labels: list[str]) -> str:
    unique_tokens = set(tokenize(text))
    scores = {label: 0.0 for label in labels}
    for token in unique_tokens:
        entry = weights.get(token)
        if not entry:
            continue
        for label in labels:
            scores[label] += float(entry.get(label, 0.0))
    return max(labels, key=lambda label: (scores[label], label))


def main() -> int:
    if not VOCAB_PATH.exists():
        print(f"[FAIL] vocabulary.json not found: {VOCAB_PATH}", file=sys.stderr)
        print("  Run: python scripts/train-ripeness-model.py --corpus data/ripeness/corpus.json "
              "--output artifacts/training/ripeness/local --training-profile ci --session-mode single --max-sessions 1",
              file=sys.stderr)
        return 1

    if not CHALLENGE_PATH.exists():
        print(f"[FAIL] challenge-set.json not found: {CHALLENGE_PATH}", file=sys.stderr)
        return 1

    vocab_payload = json.loads(VOCAB_PATH.read_text(encoding="utf-8"))
    labels: list[str] = vocab_payload["labels"]
    vocab_entries: list[dict] = vocab_payload["vocabulary"]

    # Build centered-weights lookup
    weights: dict[str, dict[str, float]] = {
        entry["token"]: entry["centered_weights"] for entry in vocab_entries
    }

    challenge_payload = json.loads(CHALLENGE_PATH.read_text(encoding="utf-8"))
    prompts: list[dict] = challenge_payload["prompts"]

    correct_by_label: Counter[str] = Counter()
    total_by_label: Counter[str] = Counter()
    misses: list[dict] = []

    for prompt in prompts:
        prompt_id = prompt["id"]
        expected = prompt["label"]
        text = prompt["text"]
        predicted = predict(text, weights, labels)
        total_by_label[expected] += 1
        if predicted == expected:
            correct_by_label[expected] += 1
        else:
            misses.append({"id": prompt_id, "expected": expected, "predicted": predicted, "text": text})

    overall_correct = sum(correct_by_label.values())
    overall_total = sum(total_by_label.values())

    print(f"score-ripeness-challenge-set: {overall_correct}/{overall_total} correct "
          f"({100*overall_correct/overall_total:.0f}%)")
    print()

    failures: list[str] = []
    for label in labels:
        total = total_by_label[label]
        correct = correct_by_label[label]
        recall = correct / total if total > 0 else 0.0
        status = "PASS" if recall >= PASS_RECALL_THRESHOLD else "FAIL"
        print(f"  {label:<12} recall={recall:.2f}  ({correct}/{total})  [{status}]")
        if recall < PASS_RECALL_THRESHOLD:
            failures.append(
                f"'{label}' recall {recall:.2f} < {PASS_RECALL_THRESHOLD}. "
                f"Add more training samples using discriminative '{label}' tokens."
            )

    if misses:
        print()
        print("Misclassified prompts:")
        for miss in misses:
            print(f"  [{miss['id']}] expected={miss['expected']}  predicted={miss['predicted']}")
            print(f"    \"{miss['text']}\"")

    if failures:
        print()
        print("Remediation:")
        for msg in failures:
            print(f"  - {msg}")
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
