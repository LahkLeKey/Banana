#!/usr/bin/env python3
"""Generate staged banana curriculum candidates from ranked knowledge gaps."""

from __future__ import annotations

import argparse
import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

TEMPLATES: dict[str, list[str]] = {
    "fruit-identity": [
        "A banana is an edible fruit that grows in tropical regions.",
        "Banana is classified as a fruit and is eaten worldwide.",
    ],
    "plant-growth": [
        "Bananas grow on large herbaceous Musa plants in warm climates.",
        "A banana plant is a giant herb, not a woody tree.",
    ],
    "ripe-yellow": [
        "A ripe banana usually has a yellow peel and sweeter taste.",
        "Banana peel color changes from green to yellow as it ripens.",
    ],
    "edible-sweet": [
        "Banana flesh is soft, edible, and naturally sweet.",
        "People peel bananas and eat the sweet fruit inside.",
    ],
    "peel-structure": [
        "Bananas have an outer peel that is removed before eating.",
        "The banana peel protects the edible fruit inside.",
    ],
    "nutrition-potassium": [
        "Bananas contain potassium and other useful nutrients.",
        "A banana is known for potassium content in a healthy diet.",
    ],
}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate banana curriculum candidates")
    parser.add_argument(
        "--gaps",
        type=Path,
        default=Path("artifacts/training/banana/local/knowledge-gaps.json"),
        help="Knowledge gaps report path",
    )
    parser.add_argument(
        "--staging",
        type=Path,
        default=Path("data/banana/curriculum/staging.json"),
        help="Staging file path",
    )
    parser.add_argument("--max-new", type=int, default=10, help="Max new candidates to add")
    return parser.parse_args()


def utc_now() -> str:
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def main() -> int:
    args = parse_args()

    gaps_payload = json.loads(args.gaps.read_text(encoding="utf-8"))
    gaps = gaps_payload.get("gaps", [])
    if not isinstance(gaps, list):
        raise ValueError(f"{args.gaps}: gaps must be a list")

    staging = json.loads(args.staging.read_text(encoding="utf-8"))
    entries = staging.get("entries", [])
    if not isinstance(entries, list):
        raise ValueError(f"{args.staging}: entries must be a list")

    max_auto = int(staging.get("max_auto_accept_per_run", max(1, args.max_new)))
    allowed_new = max(0, min(int(args.max_new), max_auto))

    seen_texts = {str(item.get("text", "")).strip().lower() for item in entries if isinstance(item, dict)}
    created = 0

    for gap in gaps:
        if created >= allowed_new:
            break
        if not isinstance(gap, dict):
            continue
        concept_id = str(gap.get("concept_id", "")).strip()
        if not concept_id:
            continue
        templates = TEMPLATES.get(concept_id, [])
        for template in templates:
            if created >= allowed_new:
                break
            key = template.strip().lower()
            if key in seen_texts:
                continue
            entries.append(
                {
                    "id": f"curr-{concept_id}-{len(entries)+1:04d}",
                    "label": "banana",
                    "text": template,
                    "concepts": [concept_id],
                    "status": "staged",
                    "source": "autonomous-curriculum-v1",
                    "provenance": {
                        "gap_priority_score": float(gap.get("priority_score", 0.0)),
                        "generated_at_utc": utc_now(),
                        "lineage_id": f"gap-{concept_id}",
                    },
                }
            )
            seen_texts.add(key)
            created += 1

    staging["entries"] = entries
    args.staging.write_text(json.dumps(staging, indent=2) + "\n", encoding="utf-8")

    summary = {
        "staging": str(args.staging),
        "created": created,
        "total_entries": len(entries),
        "max_new_effective": allowed_new,
    }
    print(json.dumps(summary, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
