#!/usr/bin/env python3
"""Score banana grade-school explanations against required fact rubric."""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path
from typing import Any

WORD_RE = re.compile(r"[A-Za-z0-9']+")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Score banana explanation quality")
    parser.add_argument(
        "--rubric",
        type=Path,
        default=Path("data/banana/explanation-rubric.json"),
        help="Path to explanation rubric",
    )
    parser.add_argument(
        "--prompts",
        type=Path,
        default=Path("data/banana/explanation-prompts.json"),
        help="Path to explanation prompt/result items",
    )
    parser.add_argument(
        "--report",
        type=Path,
        default=Path("artifacts/training/banana/local/explanation-quality.json"),
        help="Path to write quality report",
    )
    parser.add_argument("--strict", action="store_true", help="Fail on missing required fact coverage")
    return parser.parse_args()


def words(text: str) -> list[str]:
    return WORD_RE.findall(text.lower())


def contains_any(text: str, options: list[str]) -> bool:
    lowered = text.lower()
    return any(opt.lower() in lowered for opt in options)


def score_item(item: dict[str, Any], required_facts: list[dict[str, Any]]) -> dict[str, Any]:
    explanation = str(item.get("candidate_explanation", "")).strip()
    fact_hits: dict[str, bool] = {}
    for fact in required_facts:
        fact_id = str(fact.get("id", "unknown")).strip() or "unknown"
        terms = [str(term).strip() for term in fact.get("required_terms", []) if str(term).strip()]
        fact_hits[fact_id] = contains_any(explanation, terms)

    word_count = len(words(explanation))
    covered = sum(1 for ok in fact_hits.values() if ok)
    total = len(fact_hits)
    return {
        "id": str(item.get("id", "")),
        "word_count": word_count,
        "fact_hits": fact_hits,
        "fact_coverage": 0.0 if total == 0 else round(covered / total, 6),
    }


def main() -> int:
    args = parse_args()
    rubric = json.loads(args.rubric.read_text(encoding="utf-8"))
    prompts_payload = json.loads(args.prompts.read_text(encoding="utf-8"))

    required_facts = rubric.get("required_facts", [])
    prompts = prompts_payload.get("items", [])
    if not isinstance(required_facts, list) or not required_facts:
        raise ValueError(f"{args.rubric}: required_facts must be non-empty")
    if not isinstance(prompts, list) or not prompts:
        raise ValueError(f"{args.prompts}: items must be non-empty")

    min_words = int(rubric.get("min_words", 1))
    max_words = int(rubric.get("max_words", 9999))
    strict_required_facts = int(rubric.get("strict_required_facts", len(required_facts)))

    item_scores: list[dict[str, Any]] = []
    fact_totals = {str(f.get("id", "unknown")): 0 for f in required_facts}
    length_failures = 0
    strict_failures = 0

    for item in prompts:
        if not isinstance(item, dict):
            continue
        score = score_item(item, required_facts)
        score["length_ok"] = min_words <= int(score["word_count"]) <= max_words
        if not score["length_ok"]:
            length_failures += 1
        covered_count = sum(1 for ok in score["fact_hits"].values() if ok)
        score["strict_required_facts_ok"] = covered_count >= strict_required_facts
        if not score["strict_required_facts_ok"]:
            strict_failures += 1
        for key, ok in score["fact_hits"].items():
            if ok:
                fact_totals[key] = fact_totals.get(key, 0) + 1
        item_scores.append(score)

    total_items = len(item_scores)
    fact_coverage_by_id = {
        key: round((count / total_items) if total_items else 0.0, 6)
        for key, count in sorted(fact_totals.items())
    }
    avg_word_count = round(
        (sum(int(score["word_count"]) for score in item_scores) / total_items) if total_items else 0.0,
        3,
    )

    summary = {
        "schema_version": 1,
        "rubric_path": str(args.rubric),
        "prompts_path": str(args.prompts),
        "item_count": total_items,
        "fact_coverage_by_id": fact_coverage_by_id,
        "average_word_count": avg_word_count,
        "min_words": min_words,
        "max_words": max_words,
        "length_failures": length_failures,
        "strict_required_facts": strict_required_facts,
        "strict_failures": strict_failures,
        "items": item_scores,
    }

    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))

    if args.strict and (length_failures > 0 or strict_failures > 0):
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
