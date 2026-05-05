#!/usr/bin/env python3
"""Build a contrastive replay pack from reinforcement disagreement entries."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Any


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Build contrastive replay pack")
    parser.add_argument(
        "--feedback",
        type=Path,
        default=Path("data/not-banana/feedback/inbox.json"),
        help="Feedback inbox path",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=Path("data/not-banana/replay/contrastive-replay-pack.json"),
        help="Output replay pack path",
    )
    parser.add_argument("--max-entries", type=int, default=50, help="Max replay entries to include")
    return parser.parse_args()


def normalize_label(label: Any) -> str | None:
    raw = str(label).strip().lower().replace("_", "-")
    if raw == "banana":
        return "banana"
    if raw in {"not-banana", "not banana", "notbanana"}:
        return "not-banana"
    return None


def main() -> int:
    args = parse_args()
    payload = json.loads(args.feedback.read_text(encoding="utf-8"))
    entries = payload if isinstance(payload, list) else payload.get("entries", [])
    if not isinstance(entries, list):
        raise ValueError(f"{args.feedback}: expected list or object.entries list")

    replay_entries: list[dict[str, Any]] = []
    seen: set[tuple[str, str]] = set()

    for item in entries:
        if len(replay_entries) >= max(0, int(args.max_entries)):
            break
        if not isinstance(item, dict):
            continue
        if not bool(item.get("disagreement", False)):
            continue

        payload_obj = item.get("payload")
        if not isinstance(payload_obj, dict):
            continue
        text = str(payload_obj.get("text", "")).strip()
        label = normalize_label(item.get("label"))
        if not text or label is None:
            continue

        key = (label, text.lower())
        if key in seen:
            continue
        seen.add(key)

        replay_entries.append(
            {
                "id": str(item.get("id", f"replay-{len(replay_entries)+1:03d}")),
                "label": label,
                "text": text,
                "source": "reinforcement-disagreement",
                "provenance": {
                    "session_id": str(item.get("session_id", "")),
                    "predicted_label": str(item.get("predicted_label", "")),
                },
            }
        )

    output_payload = {
        "schema_version": 1,
        "description": "Contrastive replay pack extracted from disagreement feedback.",
        "entry_count": len(replay_entries),
        "entries": replay_entries,
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(output_payload, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"output": str(args.output), "entry_count": len(replay_entries)}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
