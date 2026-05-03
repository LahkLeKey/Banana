#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

specs=(
  "141-vercel-redirects-www-domain"
  "142-vercel-deploy-hooks"
  "143-vercel-logs-inspection"
  "145-vercel-edge-middleware"
)

for spec in "${specs[@]}"; do
  echo "[vercel-scaffold] scaffolding $spec"
  bash scripts/scaffold-vercel-spec.sh "$spec"
  python scripts/validate-spec-tasks-parity.py ".specify/specs/$spec"
done

python - <<'PY'
import json
from pathlib import Path

json.loads(Path("vercel.json").read_text(encoding="utf-8"))
print("[vercel-scaffold] vercel.json OK")
PY