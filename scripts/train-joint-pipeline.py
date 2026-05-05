#!/usr/bin/env python3
"""BananaAI harness for orchestrating all model lanes as one coordinated run."""

from __future__ import annotations

import argparse
import json
import shutil
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
    parser = argparse.ArgumentParser(description="BananaAI multi-model training harness.")
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
    parser.add_argument(
        "--promote-not-banana-runtime",
        action="store_true",
        help=(
            "Copy not-banana artifacts from artifacts/training/not-banana/local into "
            "runtime-discovered artifact locations to reduce builtin fallback usage."
        ),
    )
    parser.add_argument(
        "--runtime-not-banana-dirs",
        nargs="+",
        default=[
            "artifacts/not-banana-model-readiness-check",
            "artifacts/not-banana-model",
        ],
        help="Destination directories that should receive promoted not-banana artifacts.",
    )
    parser.add_argument(
        "--sync-native-header",
        action="store_true",
        help=(
            "When promoting runtime artifacts, also copy banana_signal_tokens.h into "
            "src/native/core/domain/generated for native/runtime contract alignment."
        ),
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


def load_metrics_summary(model_output_dir: Path) -> dict:
    metrics_path = model_output_dir / "metrics.json"
    if not metrics_path.exists():
        return {"metrics_path": str(metrics_path), "present": False}

    try:
        payload = json.loads(metrics_path.read_text(encoding="utf-8"))
        metrics = payload.get("metrics", {}) if isinstance(payload, dict) else {}
        if not isinstance(metrics, dict):
            metrics = {}
        return {
            "metrics_path": str(metrics_path),
            "present": True,
            "meets_thresholds": metrics.get("meets_thresholds"),
            "signal_score": metrics.get("signal_score"),
            "holdout_f1": metrics.get("holdout_f1"),
            "macro_f1": metrics.get("macro_f1"),
            "holdout_accuracy": metrics.get("holdout_accuracy"),
            "training_profile": metrics.get("training_profile"),
            "session_mode": metrics.get("session_mode"),
            "evaluated_sessions": metrics.get("evaluated_sessions"),
            "selected_session": metrics.get("selected_session"),
        }
    except (OSError, json.JSONDecodeError) as error:
        return {
            "metrics_path": str(metrics_path),
            "present": False,
            "error": f"metrics_parse_error:{error}",
        }


def promote_not_banana_runtime_artifacts(
    source_dir: Path,
    runtime_dirs: list[Path],
    sync_native_header: bool,
) -> dict:
    required_files = [
        "vocabulary.json",
        "metrics.json",
        "sessions.json",
        "banana_signal_tokens.h",
    ]

    missing = [name for name in required_files if not (source_dir / name).exists()]
    if missing:
        return {
            "attempted": True,
            "promoted": False,
            "source": str(source_dir),
            "error": f"missing_required_files:{','.join(missing)}",
        }

    copied_destinations: list[str] = []
    for runtime_dir in runtime_dirs:
        runtime_dir.mkdir(parents=True, exist_ok=True)
        for name in required_files:
            shutil.copy2(source_dir / name, runtime_dir / name)
        copied_destinations.append(str(runtime_dir))

    native_header_target: str | None = None
    if sync_native_header:
        generated_dir = Path("src/native/core/domain/generated")
        generated_dir.mkdir(parents=True, exist_ok=True)
        target = generated_dir / "banana_signal_tokens.h"
        shutil.copy2(source_dir / "banana_signal_tokens.h", target)
        native_header_target = str(target)

    return {
        "attempted": True,
        "promoted": True,
        "source": str(source_dir),
        "destinations": copied_destinations,
        "native_header_target": native_header_target,
    }


def main() -> None:
    args = parse_args()
    corpus_dir = Path(args.corpus_dir)
    output_dir = Path(args.output)
    output_dir.mkdir(parents=True, exist_ok=True)

    session_id = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    session_meta: dict = {
        "session_id": session_id,
        "harness": "banana-ai",
        "training_profile": args.training_profile,
        "session_mode": args.session_mode,
        "max_sessions": args.max_sessions,
        "corpus_dir": str(corpus_dir),
        "results": [],
        "model_metrics": {},
    }

    all_success = True
    for trainer in TRAINERS:
        corpus_path = corpus_dir / trainer["corpus"].split("/", 1)[1]
        trainer_output = str(
            Path("artifacts/training") / trainer["output_subdir"] / "local"
        )
        trainer_output_path = Path(trainer_output)
        result = run_trainer(
            script=trainer["script"],
            corpus=str(corpus_path),
            output=trainer_output,
            training_profile=args.training_profile,
            session_mode=args.session_mode,
            max_sessions=args.max_sessions,
        )
        result["model"] = trainer["name"]
        result["output"] = str(trainer_output_path)
        session_meta["results"].append(result)
        session_meta["model_metrics"][trainer["name"]] = load_metrics_summary(trainer_output_path)
        if not result["success"]:
            all_success = False
            print(
                f"[joint] WARNING: {trainer['name']} trainer exited with code {result['returncode']}",
                flush=True,
            )

    if args.promote_not_banana_runtime:
        source_dir = Path("artifacts/training/not-banana/local")
        runtime_dirs = [Path(path) for path in args.runtime_not_banana_dirs]
        promotion = promote_not_banana_runtime_artifacts(
            source_dir=source_dir,
            runtime_dirs=runtime_dirs,
            sync_native_header=args.sync_native_header,
        )
        session_meta["not_banana_runtime_promotion"] = promotion
        if not promotion.get("promoted", False):
            all_success = False
            print(
                "[joint] WARNING: not-banana runtime promotion did not complete successfully",
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
