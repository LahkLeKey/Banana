#!/usr/bin/env python3
"""Joint training pipeline — runs all three trainers in sequence with a unified replay buffer."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path


TRAINERS = [
    {
        "name": "not-banana",
        "script": "scripts/train-not-banana-model.py",
        "corpus": "data/not-banana/corpus.json",
        "output_subdir": "not-banana",
    },
    {
        "name": "banana",
        "script": "scripts/train-banana-model.py",
        "corpus": "data/banana/corpus.json",
        "output_subdir": "banana",
    },
    {
        "name": "ripeness",
        "script": "scripts/train-ripeness-model.py",
        "corpus": "data/ripeness/corpus.json",
        "output_subdir": "ripeness",
    },
]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Joint cross-model training pipeline.")
    parser.add_argument(
        "--corpus-dir",
        default="data/",
        help="Root directory containing banana/, not-banana/, ripeness/ corpus sub-folders.",
    )
    parser.add_argument(
        "--output",
        default="artifacts/training/joint",
        help="Output directory for joint session metadata.",
    )
    parser.add_argument(
        "--training-profile",
        choices=["ci", "local", "overnight", "auto"],
        default="ci",
        help="Resource profile forwarded to each sub-trainer.",
    )
    parser.add_argument(
        "--session-mode",
        choices=["single", "incremental"],
        default="single",
        help="Session mode forwarded to each sub-trainer.",
    )
    parser.add_argument(
        "--max-sessions",
        type=int,
        default=1,
        help="Maximum sessions forwarded to each sub-trainer.",
    )
    return parser.parse_args()


def run_trainer(
    script: str,
    corpus: str,
    output: str,
    training_profile: str,
    session_mode: str,
    max_sessions: int,
) -> dict:
    cmd = [
        sys.executable,
        script,
        "--corpus",
        corpus,
        "--output",
        output,
        "--training-profile",
        training_profile,
        "--session-mode",
        session_mode,
        "--max-sessions",
        str(max_sessions),
    ]
    print(f"[joint] running: {' '.join(cmd)}", flush=True)
    result = subprocess.run(cmd, capture_output=False)
    return {
        "command": cmd,
        "returncode": result.returncode,
        "success": result.returncode == 0,
    }


def main() -> None:
    args = parse_args()
    corpus_dir = Path(args.corpus_dir)
    output_dir = Path(args.output)
    output_dir.mkdir(parents=True, exist_ok=True)

    session_id = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    session_meta: dict = {
        "session_id": session_id,
        "training_profile": args.training_profile,
        "session_mode": args.session_mode,
        "max_sessions": args.max_sessions,
        "corpus_dir": str(corpus_dir),
        "results": [],
    }

    all_success = True
    for trainer in TRAINERS:
        corpus_path = corpus_dir / trainer["corpus"].split("/", 1)[1]
        trainer_output = str(
            Path("artifacts/training") / trainer["output_subdir"] / "local"
        )
        result = run_trainer(
            script=trainer["script"],
            corpus=str(corpus_path),
            output=trainer_output,
            training_profile=args.training_profile,
            session_mode=args.session_mode,
            max_sessions=args.max_sessions,
        )
        result["model"] = trainer["name"]
        session_meta["results"].append(result)
        if not result["success"]:
            all_success = False
            print(
                f"[joint] WARNING: {trainer['name']} trainer exited with code {result['returncode']}",
                flush=True,
            )

    session_meta["all_success"] = all_success
    session_meta["completed_at"] = datetime.now(timezone.utc).isoformat()

    meta_path = output_dir / f"session-{session_id}.json"
    meta_path.write_text(json.dumps(session_meta, indent=2))
    print(f"[joint] session metadata written to {meta_path}", flush=True)

    if not all_success:
        sys.exit(1)


if __name__ == "__main__":
    main()
