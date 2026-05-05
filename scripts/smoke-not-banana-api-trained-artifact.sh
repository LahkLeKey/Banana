#!/usr/bin/env bash
set -euo pipefail

workspace_root="$(cd "${1:-$(pwd)}" && pwd)"
api_dir="${workspace_root}/src/typescript/api"
api_log="${workspace_root}/artifacts/training/joint/not-banana-api-smoke.log"
mkdir -p "$(dirname "$api_log")"

artifact_path="${BANANA_NOT_BANANA_MODEL_PATH:-${workspace_root}/artifacts/not-banana-model-readiness-check/vocabulary.json}"

if [[ ! -f "$artifact_path" ]]; then
  echo "[smoke] missing artifact path: $artifact_path" >&2
  exit 1
fi

if [[ ! -d "$api_dir" ]]; then
  echo "[smoke] missing API directory: $api_dir" >&2
  exit 1
fi

if [[ ! -d "$api_dir/node_modules" ]] || [[ ! -d "$api_dir/node_modules/@fastify/rate-limit" ]]; then
  echo "[smoke] installing API dependencies via bun install"
  (
    cd "$api_dir"
    bun install --frozen-lockfile
  )
fi
response_file="${workspace_root}/artifacts/training/joint/not-banana-model-response.json"
mkdir -p "$(dirname "$response_file")"

(
  cd "$api_dir"
  BANANA_NOT_BANANA_MODEL_PATH="$artifact_path" bun --eval '
import Fastify from "fastify";
import { registerNotBananaRoutes } from "./src/domains/not-banana/routes.ts";

const app = Fastify({ logger: false });
await registerNotBananaRoutes(app);
await app.ready();

const res = await app.inject({ method: "GET", url: "/not-banana/model" });
if (res.statusCode !== 200) {
  throw new Error(`unexpected_status:${res.statusCode}`);
}

console.log(res.body);
await app.close();
' >"$response_file" 2>"$api_log"
)

python - "$response_file" <<'PY'
import json
import sys
from pathlib import Path

path = Path(sys.argv[1])
payload = json.loads(path.read_text(encoding="utf-8"))
source = payload.get("source")
if source != "trained-artifact":
    fallback = payload.get("fallback_reason")
    raise SystemExit(
        f"[smoke] expected source=trained-artifact, got source={source!r}, fallback_reason={fallback!r}"
    )

print(json.dumps({
    "ok": True,
    "source": source,
    "artifact_path": payload.get("artifact_path"),
    "recommended_threshold": payload.get("recommended_threshold"),
}, indent=2))
PY

echo "[smoke] not-banana endpoint validated with trained artifact"
