#!/usr/bin/env python3
"""Apply not-banana feedback entries into the training corpus."""

from __future__ import annotations

import argparse
import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


VALID_LABELS = {"BANANA", "NOT_BANANA"}


def utc_now() -> str:
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def load_json_object(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise ValueError(f"Expected JSON object at {path}.")
    return data


def write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=4) + "\n", encoding="utf-8")


def canonical_sample(label: str, payload: dict[str, Any]) -> str:
    return json.dumps(
        {"label": label, "payload": payload},
        sort_keys=True,
        separators=(",", ":"),
        ensure_ascii=False,
    )


def parse_status_filter(raw: str) -> set[str]:
    parsed = {item.strip().lower() for item in raw.split(",") if item.strip()}
    if not parsed:
        raise ValueError("status filter must contain at least one value")
    return parsed


def add_bool_arg(parser: argparse.ArgumentParser, name: str, default: bool, help_text: str) -> None:
    parser.add_argument(
        name,
        default=default,
        action=argparse.BooleanOptionalAction,
        help=help_text,
    )


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Apply not-banana feedback entries into corpus.json")
    parser.add_argument(
        "--corpus",
        type=Path,
        default=Path("data/not-banana/corpus.json"),
        help="Path to corpus.json",
    )
    parser.add_argument(
        "--feedback",
        type=Path,
        default=Path("data/not-banana/feedback/inbox.json"),
        help="Path to feedback inbox file",
    )
    parser.add_argument(
        "--report",
        type=Path,
        default=Path("artifacts/not-banana-feedback/apply-report.json"),
        help="Path to write apply report JSON",
    )
    parser.add_argument(
        "--status-filter",
        default="approved",
        help="Comma-separated feedback statuses to ingest",
    )
    parser.add_argument(
        "--max-entries",
        type=int,
        default=0,
        help="Maximum number of eligible entries to process (0 means no limit)",
    )
    add_bool_arg(
        parser,
        "--consume",
        True,
        "Mark processed feedback entries as applied/error to avoid repeat ingestion",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Calculate and report changes without writing files",
    )
    parser.add_argument(
        "--strict",
        action="store_true",
        help="Fail if any eligible feedback entry is invalid",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    status_filter = parse_status_filter(args.status_filter)
    timestamp = utc_now()

    corpus = load_json_object(args.corpus)
    samples_raw = corpus.get("samples")
    if not isinstance(samples_raw, list) or not samples_raw:
        raise ValueError(f"Corpus at {args.corpus} must contain a non-empty 'samples' array.")

    existing_keys: set[str] = set()
    for index, sample in enumerate(samples_raw):
        if not isinstance(sample, dict):
            raise ValueError(f"Corpus sample #{index} must be an object.")
        label = sample.get("label")
        payload = sample.get("payload")
        if label not in VALID_LABELS or not isinstance(payload, dict):
            raise ValueError(f"Corpus sample #{index} must contain valid label and payload object.")
        existing_keys.add(canonical_sample(str(label), payload))

    feedback = load_json_object(args.feedback)
    entries = feedback.get("entries")
    if not isinstance(entries, list):
        raise ValueError(f"Feedback inbox at {args.feedback} must contain an 'entries' array.")

    total_entries = len(entries)
    corpus_count_before = len(samples_raw)

    eligible_count = 0
    processed_count = 0
    added_count = 0
    duplicate_count = 0
    invalid_count = 0
    consumed_updates = 0

    invalid_entries: list[dict[str, Any]] = []

    for index, entry in enumerate(entries):
        if not isinstance(entry, dict):
            continue

        status = str(entry.get("status", "pending")).strip().lower()
        if status not in status_filter:
            continue

        eligible_count += 1
        if args.max_entries > 0 and processed_count >= args.max_entries:
            continue

        processed_count += 1
        feedback_id = str(entry.get("id", f"entry-{index}"))
        label = entry.get("label")
        payload = entry.get("payload")

        if label not in VALID_LABELS or not isinstance(payload, dict):
            invalid_count += 1
            invalid_entries.append(
                {
                    "id": feedback_id,
                    "index": index,
                    "reason": "entry must contain label (BANANA or NOT_BANANA) and payload object",
                }
            )

            if args.consume and not args.dry_run:
                entry["status"] = "error"
                entry["error"] = "invalid label or payload"
                entry["last_processed_at_utc"] = timestamp
                consumed_updates += 1
            continue

        sample_key = canonical_sample(str(label), payload)
        result = "duplicate" if sample_key in existing_keys else "added"

        if result == "added":
            added_count += 1
            if not args.dry_run:
                samples_raw.append({"label": str(label), "payload": payload})
                existing_keys.add(sample_key)
        else:
            duplicate_count += 1

        if args.consume and not args.dry_run:
            entry["status"] = "applied"
            entry["apply_result"] = result
            entry["applied_at_utc"] = timestamp
            consumed_updates += 1

    report: dict[str, Any] = {
        "updated_at_utc": timestamp,
        "dry_run": bool(args.dry_run),
        "strict": bool(args.strict),
        "consume": bool(args.consume),
        "status_filter": sorted(status_filter),
        "max_entries": int(args.max_entries),
        "corpus_path": str(args.corpus),
        "feedback_path": str(args.feedback),
        "feedback_entry_count": total_entries,
        "eligible_count": eligible_count,
        "processed_count": processed_count,
        "added_count": added_count,
        "duplicate_count": duplicate_count,
        "invalid_count": invalid_count,
        "consumed_updates": consumed_updates,
        "corpus_sample_count_before": corpus_count_before,
        "corpus_sample_count_after": corpus_count_before + added_count,
        "invalid_entries": invalid_entries,
    }

    report_parent = args.report.parent
    report_parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")

    if args.strict and invalid_count > 0:
        print(f"::error::{invalid_count} invalid feedback entries encountered. See {args.report}.")
        return 1

    if args.dry_run:
        print(
            "Feedback dry-run summary: "
            f"eligible={eligible_count} processed={processed_count} "
            f"added={added_count} duplicate={duplicate_count} invalid={invalid_count}"
        )
        return 0

    files_written = 0
    if added_count > 0:
        write_json(args.corpus, corpus)
        files_written += 1

    if args.consume and consumed_updates > 0:
        write_json(args.feedback, feedback)
        files_written += 1

    if files_written == 0:
        print("No feedback updates were applied.")
    else:
        print(
            "Feedback apply summary: "
            f"added={added_count} duplicate={duplicate_count} "
            f"invalid={invalid_count} files_written={files_written}"
        )

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
