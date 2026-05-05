#!/usr/bin/env python3
"""Auto-enrich banana/not-banana corpora from Wikidata entity metadata.

This script reduces manual corpus curation by:
1. Discovering entities from query seeds using Wikidata search.
2. Fetching labels/descriptions/aliases for each entity.
3. Building normalized training text snippets.
4. Deduplicating against existing corpus samples.
5. Appending high-confidence samples to corpus.json files.
"""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
import urllib.parse
import urllib.request
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

SEARCH_ENDPOINT = "https://www.wikidata.org/w/api.php"
DEFAULT_BANANA_QUERIES = [
    "banana",
    "plantain",
    "banana cultivar",
    "banana dish",
    "banana agriculture",
]
DEFAULT_NOT_BANANA_QUERIES = [
    "bicycle",
    "keyboard",
    "weather radar",
    "contract law",
    "logistics",
    "camera",
    "network router",
    "coffee mug",
]
NORMALIZE_SPACE_RE = re.compile(r"\s+")
BANANA_TOKEN_RE = re.compile(r"\bbanana\b", re.IGNORECASE)
BANANA_ALLOWED_RE = re.compile(
    r"\b(banana|plantain|musa)\b",
    re.IGNORECASE,
)
BANANA_CONTEXT_RE = re.compile(
    r"\b(fruit|edible|cultivar|dish|food|harvest|agriculture|plantation|cooking|vegetable|genus)\b",
    re.IGNORECASE,
)
PLANTAIN_FOOD_CONTEXT_RE = re.compile(
    r"\b(cooking|food|fruit|vegetable|cultivar|edible|banana)\b",
    re.IGNORECASE,
)
BANANA_BLOCK_RE = re.compile(
    r"\b(given name|family name|tv series|pop group|song|seaport|local government area|"
    r"clinical trial|scientific article|video game|painting|drawing|film|album|"
    r"municipality|district|village|city|port|band)\b",
    re.IGNORECASE,
)
NOT_BANANA_BLOCK_RE = re.compile(
    r"\b(given name|family name|song|album|film|tv series|webcomic|comic strip|"
    r"painting|drawing|fictional character|video game|disambiguation page)\b",
    re.IGNORECASE,
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Enrich banana corpora from Wikidata metadata."
    )
    parser.add_argument(
        "--banana-corpus",
        type=Path,
        default=Path("data/banana/corpus.json"),
        help="Path to banana corpus JSON.",
    )
    parser.add_argument(
        "--not-banana-corpus",
        type=Path,
        default=Path("data/not-banana/corpus.json"),
        help="Path to not-banana corpus JSON.",
    )
    parser.add_argument(
        "--banana-queries",
        nargs="*",
        default=DEFAULT_BANANA_QUERIES,
        help="Wikidata query seeds for banana samples.",
    )
    parser.add_argument(
        "--not-banana-queries",
        nargs="*",
        default=DEFAULT_NOT_BANANA_QUERIES,
        help="Wikidata query seeds for not-banana samples.",
    )
    parser.add_argument(
        "--search-limit",
        type=int,
        default=20,
        help="Max entities per query seed.",
    )
    parser.add_argument(
        "--max-add-per-label",
        type=int,
        default=16,
        help="Maximum new samples appended per label in one run.",
    )
    parser.add_argument(
        "--min-text-length",
        type=int,
        default=24,
        help="Minimum text length for generated samples.",
    )
    parser.add_argument(
        "--report",
        type=Path,
        default=Path("artifacts/corpus-enrichment/wikidata-report.json"),
        help="Path for run report JSON.",
    )
    parser.add_argument(
        "--timeout-seconds",
        type=float,
        default=15.0,
        help="HTTP timeout for Wikidata API requests.",
    )
    parser.add_argument(
        "--max-per-query",
        type=int,
        default=3,
        help="Maximum accepted candidates taken from each query seed.",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Compute candidates and report without writing corpus files.",
    )
    parser.add_argument(
        "--skip-schema-validate",
        action="store_true",
        help="Skip automatic scripts/check-corpus-schema.py validation after writes.",
    )
    parser.add_argument(
        "--prune-source",
        nargs="*",
        default=[],
        help="Remove existing samples matching any listed source values before append.",
    )
    return parser.parse_args()


def utc_now() -> str:
    return (
        datetime.now(timezone.utc)
        .replace(microsecond=0)
        .isoformat()
        .replace("+00:00", "Z")
    )


def normalize_text(value: str) -> str:
    collapsed = NORMALIZE_SPACE_RE.sub(" ", value.strip().lower())
    return collapsed


def load_corpus(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise ValueError(f"{path}: root must be an object")
    samples = payload.get("samples")
    if not isinstance(samples, list):
        raise ValueError(f"{path}: missing 'samples' array")
    return payload


def write_json(path: Path, payload: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=4) + "\n", encoding="utf-8")


def existing_text_set(corpus: dict[str, Any]) -> set[str]:
    out: set[str] = set()
    for sample in corpus.get("samples", []):
        if not isinstance(sample, dict):
            continue
        text = sample.get("text")
        if isinstance(text, str) and text.strip():
            out.add(normalize_text(text))
    return out


def preferred_id_prefix(corpus: dict[str, Any], label: str) -> str:
    if label == "banana":
        return "banana-"

    counts: dict[str, int] = {"not-": 0, "not-banana-": 0}
    for sample in corpus.get("samples", []):
        if not isinstance(sample, dict):
            continue
        if sample.get("label") != label:
            continue
        sample_id = sample.get("id")
        if not isinstance(sample_id, str):
            continue
        for prefix in counts:
            if sample_id.startswith(prefix):
                counts[prefix] += 1

    if counts["not-"] >= counts["not-banana-"]:
        return "not-"
    return "not-banana-"


def next_sample_id(corpus: dict[str, Any], label: str, prefix: str) -> int:
    max_seen = 0
    for sample in corpus.get("samples", []):
        if not isinstance(sample, dict):
            continue
        sample_id = sample.get("id")
        if not isinstance(sample_id, str):
            continue
        if sample.get("label") != label or not sample_id.startswith(prefix):
            continue
        suffix = sample_id[len(prefix) :]
        if suffix.isdigit():
            max_seen = max(max_seen, int(suffix))
    return max_seen + 1


def prune_samples_by_source(corpus: dict[str, Any], sources: list[str]) -> int:
    samples = corpus.get("samples")
    if not isinstance(samples, list) or not sources:
        return 0

    source_set = {item for item in sources if item}
    if not source_set:
        return 0

    original = len(samples)
    kept: list[dict[str, Any]] = []
    for sample in samples:
        if not isinstance(sample, dict):
            kept.append(sample)
            continue
        if sample.get("source") in source_set:
            continue
        kept.append(sample)
    corpus["samples"] = kept
    return original - len(kept)


def build_query_url(params: dict[str, Any]) -> str:
    encoded = urllib.parse.urlencode(params)
    return f"{SEARCH_ENDPOINT}?{encoded}"


def fetch_json(url: str, timeout_seconds: float) -> dict[str, Any]:
    req = urllib.request.Request(
        url,
        headers={
            "User-Agent": "banana-corpus-enrichment/1.0 (https://github.com/LahkLeKey/Banana)",
        },
    )
    with urllib.request.urlopen(req, timeout=timeout_seconds) as response:
        payload = json.loads(response.read().decode("utf-8"))
    if not isinstance(payload, dict):
        raise ValueError("Expected JSON object response")
    return payload


def search_entities(query: str, limit: int, timeout_seconds: float) -> list[str]:
    params = {
        "action": "wbsearchentities",
        "format": "json",
        "language": "en",
        "type": "item",
        "limit": max(1, min(limit, 50)),
        "search": query,
        "origin": "*",
    }
    payload = fetch_json(build_query_url(params), timeout_seconds)
    results = payload.get("search")
    if not isinstance(results, list):
        return []

    ids: list[str] = []
    seen: set[str] = set()
    for row in results:
        if not isinstance(row, dict):
            continue
        entity_id = row.get("id")
        if isinstance(entity_id, str) and entity_id.startswith("Q") and entity_id not in seen:
            seen.add(entity_id)
            ids.append(entity_id)
    return ids


def fetch_entities(entity_ids: list[str], timeout_seconds: float) -> list[dict[str, Any]]:
    if not entity_ids:
        return []

    params = {
        "action": "wbgetentities",
        "format": "json",
        "ids": "|".join(entity_ids[:50]),
        "languages": "en",
        "props": "labels|descriptions|aliases",
        "origin": "*",
    }
    payload = fetch_json(build_query_url(params), timeout_seconds)
    entities = payload.get("entities")
    if not isinstance(entities, dict):
        return []

    out: list[dict[str, Any]] = []
    for entity_id, value in entities.items():
        if not isinstance(value, dict):
            continue
        label = (
            value.get("labels", {}).get("en", {}).get("value")
            if isinstance(value.get("labels"), dict)
            else None
        )
        description = (
            value.get("descriptions", {}).get("en", {}).get("value")
            if isinstance(value.get("descriptions"), dict)
            else None
        )
        aliases_block = value.get("aliases") if isinstance(value.get("aliases"), dict) else {}
        aliases_rows = aliases_block.get("en") if isinstance(aliases_block, dict) else []
        aliases: list[str] = []
        if isinstance(aliases_rows, list):
            for row in aliases_rows[:3]:
                if isinstance(row, dict):
                    alias = row.get("value")
                    if isinstance(alias, str) and alias.strip():
                        aliases.append(alias.strip())

        out.append(
            {
                "id": entity_id,
                "label": label.strip() if isinstance(label, str) and label.strip() else None,
                "description": description.strip()
                if isinstance(description, str) and description.strip()
                else None,
                "aliases": aliases,
            }
        )
    return out


def build_training_text(entity: dict[str, Any]) -> str | None:
    label = entity.get("label")
    if not isinstance(label, str) or not label.strip():
        return None

    pieces = [label.strip()]
    description = entity.get("description")
    if isinstance(description, str) and description.strip():
        pieces.append(description.strip())

    aliases = entity.get("aliases")
    if isinstance(aliases, list):
        clean_aliases = [a.strip() for a in aliases if isinstance(a, str) and a.strip()]
        if clean_aliases:
            pieces.append("aliases " + ", ".join(clean_aliases[:2]))

    text = " ".join(pieces)
    text = NORMALIZE_SPACE_RE.sub(" ", text).strip()
    return text if text else None


def collect_candidates(
    label: str,
    queries: list[str],
    search_limit: int,
    min_text_length: int,
    timeout_seconds: float,
    max_per_query: int,
    existing: set[str],
) -> tuple[list[dict[str, Any]], list[dict[str, Any]]]:
    """Return (accepted_candidates, trace_rows)."""
    accepted: list[dict[str, Any]] = []
    trace: list[dict[str, Any]] = []

    seen_qids: set[str] = set()
    for query in queries:
        if not query.strip():
            continue

        entity_ids = search_entities(query.strip(), search_limit, timeout_seconds)
        entity_ids = [qid for qid in entity_ids if qid not in seen_qids]
        seen_qids.update(entity_ids)

        entities = fetch_entities(entity_ids, timeout_seconds)
        accepted_for_query = 0
        for entity in entities:
            if accepted_for_query >= max(1, max_per_query):
                break

            text = build_training_text(entity)
            if not text:
                trace.append({"query": query, "entity_id": entity.get("id"), "status": "skip_empty"})
                continue

            if len(text) < min_text_length:
                trace.append({
                    "query": query,
                    "entity_id": entity.get("id"),
                    "status": "skip_short",
                    "text": text,
                })
                continue

            if label == "banana":
                if not BANANA_ALLOWED_RE.search(text):
                    trace.append({
                        "query": query,
                        "entity_id": entity.get("id"),
                        "status": "skip_banana_low_relevance",
                        "text": text,
                    })
                    continue
                if not BANANA_CONTEXT_RE.search(text):
                    trace.append({
                        "query": query,
                        "entity_id": entity.get("id"),
                        "status": "skip_banana_missing_context",
                        "text": text,
                    })
                    continue
                if BANANA_BLOCK_RE.search(text):
                    trace.append({
                        "query": query,
                        "entity_id": entity.get("id"),
                        "status": "skip_banana_blocked_topic",
                        "text": text,
                    })
                    continue
                if "plantain" in text.lower() and not PLANTAIN_FOOD_CONTEXT_RE.search(text):
                    trace.append({
                        "query": query,
                        "entity_id": entity.get("id"),
                        "status": "skip_plantain_non_food",
                        "text": text,
                    })
                    continue

            if label == "not-banana" and BANANA_TOKEN_RE.search(text):
                trace.append({
                    "query": query,
                    "entity_id": entity.get("id"),
                    "status": "skip_contains_banana",
                    "text": text,
                })
                continue

            if label == "not-banana" and NOT_BANANA_BLOCK_RE.search(text):
                trace.append({
                    "query": query,
                    "entity_id": entity.get("id"),
                    "status": "skip_not_banana_blocked_topic",
                    "text": text,
                })
                continue

            key = normalize_text(text)
            if key in existing:
                trace.append({
                    "query": query,
                    "entity_id": entity.get("id"),
                    "status": "skip_duplicate",
                    "text": text,
                })
                continue

            existing.add(key)
            accepted.append(
                {
                    "entity_id": entity.get("id"),
                    "query": query,
                    "text": text,
                }
            )
            accepted_for_query += 1
            trace.append({
                "query": query,
                "entity_id": entity.get("id"),
                "status": "accepted",
                "text": text,
            })

    return accepted, trace


def append_samples(
    corpus: dict[str, Any],
    label: str,
    candidates: list[dict[str, Any]],
    max_add_per_label: int,
) -> list[dict[str, Any]]:
    samples = corpus.get("samples")
    if not isinstance(samples, list):
        raise ValueError("corpus missing samples list")

    prefix = preferred_id_prefix(corpus, label)
    start_number = next_sample_id(corpus, label, prefix)
    added: list[dict[str, Any]] = []

    for idx, candidate in enumerate(candidates[: max(0, max_add_per_label)]):
        sample_number = start_number + idx
        sample_id = f"{prefix}{sample_number:03d}"
        sample = {
            "id": sample_id,
            "label": label,
            "text": candidate["text"],
            "source": "wikidata-auto-v1",
            "metadata": {
                "entity_id": candidate.get("entity_id"),
                "query": candidate.get("query"),
                "harvested_at_utc": utc_now(),
            },
        }
        samples.append(sample)
        added.append(sample)

    return added


def main() -> int:
    args = parse_args()

    banana_corpus = load_corpus(args.banana_corpus)
    not_banana_corpus = load_corpus(args.not_banana_corpus)

    pruned_banana = prune_samples_by_source(banana_corpus, args.prune_source)
    pruned_not_banana = prune_samples_by_source(not_banana_corpus, args.prune_source)

    banana_existing = existing_text_set(banana_corpus)
    not_banana_existing = existing_text_set(not_banana_corpus)

    banana_candidates, banana_trace = collect_candidates(
        label="banana",
        queries=args.banana_queries,
        search_limit=args.search_limit,
        min_text_length=args.min_text_length,
        timeout_seconds=args.timeout_seconds,
        max_per_query=args.max_per_query,
        existing=banana_existing,
    )
    not_banana_candidates, not_banana_trace = collect_candidates(
        label="not-banana",
        queries=args.not_banana_queries,
        search_limit=args.search_limit,
        min_text_length=args.min_text_length,
        timeout_seconds=args.timeout_seconds,
        max_per_query=args.max_per_query,
        existing=not_banana_existing,
    )

    added_banana: list[dict[str, Any]] = []
    added_not_banana: list[dict[str, Any]] = []

    if not args.dry_run:
        added_banana = append_samples(
            banana_corpus,
            "banana",
            banana_candidates,
            args.max_add_per_label,
        )
        added_not_banana = append_samples(
            not_banana_corpus,
            "not-banana",
            not_banana_candidates,
            args.max_add_per_label,
        )

        write_json(args.banana_corpus, banana_corpus)
        write_json(args.not_banana_corpus, not_banana_corpus)

        if not args.skip_schema_validate:
            subprocess.run(
                [
                    sys.executable,
                    "scripts/check-corpus-schema.py",
                    str(args.banana_corpus),
                    str(args.not_banana_corpus),
                ],
                check=True,
            )

    report = {
        "schema_version": 1,
        "generated_at_utc": utc_now(),
        "dry_run": args.dry_run,
        "config": {
            "search_limit": args.search_limit,
            "max_add_per_label": args.max_add_per_label,
            "min_text_length": args.min_text_length,
            "max_per_query": args.max_per_query,
            "banana_queries": args.banana_queries,
            "not_banana_queries": args.not_banana_queries,
            "prune_source": args.prune_source,
            "skip_schema_validate": args.skip_schema_validate,
        },
        "results": {
            "banana": {
                "pruned_count": pruned_banana,
                "candidate_count": len(banana_candidates),
                "added_count": len(added_banana),
                "added_ids": [item.get("id") for item in added_banana],
            },
            "not_banana": {
                "pruned_count": pruned_not_banana,
                "candidate_count": len(not_banana_candidates),
                "added_count": len(added_not_banana),
                "added_ids": [item.get("id") for item in added_not_banana],
            },
        },
        "trace": {
            "banana": banana_trace,
            "not_banana": not_banana_trace,
        },
    }

    write_json(args.report, report)
    print(json.dumps(report["results"], indent=2))
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as error:
        print(f"wikidata enrichment failed: {error}", file=sys.stderr)
        raise
