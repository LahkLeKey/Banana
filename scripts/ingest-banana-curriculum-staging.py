#!/usr/bin/env python3
"""Ingest staged banana curriculum entries into data/banana/corpus.json with safety caps."""

from __future__ import annotations

import argparse
import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Ingest staged banana curriculum entries")
    parser.add_argument(
        "--corpus",
        type=Path,
        default=Path("data/banana/corpus.json"),
        help="Banana corpus path",
    )
    parser.add_argument(
        "--staging",
        type=Path,
        default=Path("data/banana/curriculum/staging.json"),
        help="Staging file path",
    )
    parser.add_argument(
        "--max-accept",
        type=int,
        default=10,
        help="Maximum entries accepted in one ingestion run",
    )
    parser.add_argument(
        "--report",
        type=Path,
        default=Path("artifacts/training/banana/local/curriculum-ingest-summary.json"),
        help="Summary report path",
    )
    return parser.parse_args()


def utc_now() -> str:
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def main() -> int:
    args = parse_args()

    corpus = json.loads(args.corpus.read_text(encoding="utf-8"))
    samples = corpus.get("samples", [])
    if not isinstance(samples, list):
        raise ValueError(f"{args.corpus}: samples must be a list")

    staging = json.loads(args.staging.read_text(encoding="utf-8"))
    entries = staging.get("entries", [])
    if not isinstance(entries, list):
        raise ValueError(f"{args.staging}: entries must be a list")

    max_file_cap = int(staging.get("max_auto_accept_per_run", max(1, args.max_accept)))
    accepted_cap = max(0, min(int(args.max_accept), max_file_cap))

    existing_keys = {
        (str(item.get("label", "")).strip().lower(), str(item.get("text", "")).strip().lower())
        for item in samples
        if isinstance(item, dict)
    }

    accepted = 0
    rejected = 0
    accepted_ids: list[str] = []
    rejected_ids: list[str] = []

    for entry in entries:
        if not isinstance(entry, dict):
            continue
        status = str(entry.get("status", "")).strip().lower()
        if status != "staged":
            continue

        entry_id = str(entry.get("id", "")).strip() or f"curr-{len(accepted_ids)+len(rejected_ids)+1:04d}"
        text = str(entry.get("text", "")).strip()
        label = str(entry.get("label", "banana")).strip().lower().replace("_", "-")
        if not text or label not in {"banana", "not-banana"}:
            entry["status"] = "rejected"
            entry["rejected_reason"] = "invalid label or empty text"
            rejected += 1
            rejected_ids.append(entry_id)
            continue

        key = (label, text.lower())
        if key in existing_keys:
            entry["status"] = "rejected"
            entry["rejected_reason"] = "duplicate sample"
            rejected += 1
            rejected_ids.append(entry_id)
            continue

        if accepted >= accepted_cap:
            entry["status"] = "deferred"
            entry["deferred_reason"] = "max acceptance cap reached"
            continue

        sample_id = entry_id if entry_id else f"curriculum-{len(samples)+1:04d}"
        sample: dict[str, Any] = {
            "id": sample_id,
            "label": label,
            "text": text,
            "source": "autonomous-curriculum-v1",
        }
        concepts = entry.get("concepts")
        if isinstance(concepts, list) and concepts:
            sample["concepts"] = [str(c).strip() for c in concepts if str(c).strip()]

        samples.append(sample)
        existing_keys.add(key)

        entry["status"] = "accepted"
        entry["accepted_at_utc"] = utc_now()
        accepted += 1
        accepted_ids.append(sample_id)

    corpus["samples"] = samples
    args.corpus.write_text(json.dumps(corpus, indent=2) + "\n", encoding="utf-8")
    staging["entries"] = entries
    args.staging.write_text(json.dumps(staging, indent=2) + "\n", encoding="utf-8")

    summary = {
        "schema_version": 1,
        "updated_at_utc": utc_now(),
        "corpus_path": str(args.corpus),
        "staging_path": str(args.staging),
        "accepted_count": accepted,
        "rejected_count": rejected,
        "accepted_ids": accepted_ids,
        "rejected_ids": rejected_ids,
        "acceptance_cap": accepted_cap,
    }
    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
