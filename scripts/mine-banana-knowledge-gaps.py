#!/usr/bin/env python3
"""Mine ranked banana knowledge gaps from ontology and benchmark outputs."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Any


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Mine banana knowledge gaps")
    parser.add_argument(
        "--ontology",
        type=Path,
        default=Path("data/banana/ontology-foundation.json"),
        help="Ontology definition path",
    )
    parser.add_argument(
        "--coverage-report",
        type=Path,
        default=Path("artifacts/training/banana/local/concept-coverage-summary.json"),
        help="Concept coverage report path",
    )
    parser.add_argument(
        "--generalization-report",
        type=Path,
        default=Path("artifacts/training/banana/local/generalization-benchmark.json"),
        help="Generalization report path",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=Path("artifacts/training/banana/local/knowledge-gaps.json"),
        help="Output path",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    ontology = json.loads(args.ontology.read_text(encoding="utf-8"))
    concepts = ontology.get("concepts", [])
    if not isinstance(concepts, list) or not concepts:
        raise ValueError(f"{args.ontology}: concepts must be non-empty")

    min_samples = int(ontology.get("min_samples_per_concept", 1))
    concept_ids = [str(c.get("id", "")).strip() for c in concepts if str(c.get("id", "")).strip()]

    coverage_counts: dict[str, int] = {cid: 0 for cid in concept_ids}
    if args.coverage_report.exists():
        cov = json.loads(args.coverage_report.read_text(encoding="utf-8"))
        counts = cov.get("counts", {})
        if isinstance(counts, dict):
            for cid in concept_ids:
                try:
                    coverage_counts[cid] = int(counts.get(cid, 0))
                except (TypeError, ValueError):
                    coverage_counts[cid] = 0

    lane_recall: dict[str, float] = {cid: 1.0 for cid in concept_ids}
    if args.generalization_report.exists():
        gen = json.loads(args.generalization_report.read_text(encoding="utf-8"))
        recalls = gen.get("per_concept_recall", {})
        if isinstance(recalls, dict):
            for cid in concept_ids:
                try:
                    lane_recall[cid] = float(recalls.get(cid, 1.0))
                except (TypeError, ValueError):
                    lane_recall[cid] = 1.0

    gaps: list[dict[str, Any]] = []
    for cid in concept_ids:
        count = coverage_counts.get(cid, 0)
        recall = lane_recall.get(cid, 1.0)
        coverage_deficit = max(0, min_samples - count)
        recall_deficit = max(0.0, 0.75 - recall)
        priority_score = round((coverage_deficit * 2.0) + (recall_deficit * 10.0), 6)
        if priority_score <= 0:
            continue
        gaps.append(
            {
                "concept_id": cid,
                "coverage_count": int(count),
                "min_samples": min_samples,
                "recall": round(float(recall), 6),
                "priority_score": priority_score,
            }
        )

    gaps.sort(key=lambda g: (-float(g["priority_score"]), str(g["concept_id"])))

    payload = {
        "schema_version": 1,
        "ontology_path": str(args.ontology),
        "coverage_report_path": str(args.coverage_report),
        "generalization_report_path": str(args.generalization_report),
        "gaps": gaps,
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(payload, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
