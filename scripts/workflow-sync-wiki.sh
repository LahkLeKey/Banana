#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

RUN_ID="${GITHUB_RUN_ID:-local-run}"
RUN_ATTEMPT="${GITHUB_RUN_ATTEMPT:-1}"
WIKI_DIR="${BANANA_WIKI_DIR:-.wiki}"
WIKI_REPOSITORY="${BANANA_WIKI_REPOSITORY:-${GITHUB_REPOSITORY:-}}"
WIKI_REMOTE_URL="${BANANA_WIKI_REMOTE_URL:-}"
WIKI_COMMIT_MESSAGE="${BANANA_WIKI_COMMIT_MESSAGE:-docs(wiki): sync automated SDLC documentation snapshots}"
WIKI_PUSH="${BANANA_WIKI_PUSH:-true}"
WIKI_DRY_RUN="${BANANA_WIKI_DRY_RUN:-false}"
WIKI_STRICT="${BANANA_WIKI_STRICT:-false}"
WIKI_MAPPING_RAW="${BANANA_WIKI_SYNC_MAPPING:-}"
WIKI_OUTPUT_PATH="${BANANA_WIKI_OUTPUT_PATH:-artifacts/wiki-sync/report-${RUN_ID}-attempt-${RUN_ATTEMPT}.json}"

mkdir -p "$(dirname "$WIKI_OUTPUT_PATH")"

declare -a mapping_entries=()
if [[ -n "$WIKI_MAPPING_RAW" ]]; then
  while IFS= read -r line; do
    line="${line//$'\r'/}"
    line="$(echo "$line" | xargs)"
    if [[ -z "$line" ]]; then
      continue
    fi
    if [[ "$line" == \#* ]]; then
      continue
    fi
    mapping_entries+=("$line")
  done <<< "$WIKI_MAPPING_RAW"
else
  mapping_entries=(
    "README.md|Auto-README.md"
    "docs/developer-onboarding.md|Auto-Developer-Onboarding.md"
    ".github/skills/banana-build-and-run/entry-points.md|Auto-Build-Run-Test-Entry-Points.md"
    "data/not-banana/README.md|Auto-Not-Banana-Data.md"
    "data/not-banana/feedback/README.md|Auto-Not-Banana-Feedback-Inbox.md"
  )
fi

if [[ ${#mapping_entries[@]} -eq 0 ]]; then
  echo "::warning::No wiki mapping entries supplied."
  python - "$WIKI_OUTPUT_PATH" <<'PY'
import json
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
path.parent.mkdir(parents=True, exist_ok=True)
path.write_text(json.dumps({"status": "skipped", "reason": "empty mapping"}, indent=2) + "\n", encoding="utf-8")
PY
  exit 0
fi

if [[ -z "$WIKI_REPOSITORY" ]] && command -v gh >/dev/null 2>&1; then
  if repo_name="$(gh repo view --json nameWithOwner -q .nameWithOwner 2>/dev/null)"; then
    WIKI_REPOSITORY="$repo_name"
  fi
fi

if [[ ! -d "$WIKI_DIR/.git" ]]; then
  if [[ -z "$WIKI_REMOTE_URL" ]]; then
    if [[ -z "$WIKI_REPOSITORY" ]]; then
      echo "::error::Cannot determine wiki repository slug. Set BANANA_WIKI_REPOSITORY or BANANA_WIKI_REMOTE_URL."
      exit 1
    fi
    WIKI_REMOTE_URL="https://github.com/${WIKI_REPOSITORY}.wiki.git"
  fi

  clone_url="$WIKI_REMOTE_URL"
  if [[ -n "${GH_TOKEN:-}" && "$clone_url" == https://github.com/* ]]; then
    clone_url="https://x-access-token:${GH_TOKEN}@${clone_url#https://}"
  fi

  if [[ "$WIKI_DRY_RUN" == "true" ]]; then
    mkdir -p "$WIKI_DIR"
    git -C "$WIKI_DIR" init >/dev/null 2>&1
  else
    rm -rf "$WIKI_DIR"
    if ! git clone "$clone_url" "$WIKI_DIR" >/dev/null 2>&1; then
      if [[ "$WIKI_STRICT" == "true" ]]; then
        echo "::error::Failed to clone wiki repository."
        exit 1
      fi

      echo "::warning::Wiki clone failed. Skipping wiki sync."
      python - "$WIKI_OUTPUT_PATH" <<'PY'
import json
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
path.parent.mkdir(parents=True, exist_ok=True)
path.write_text(json.dumps({"status": "skipped", "reason": "wiki clone failed"}, indent=2) + "\n", encoding="utf-8")
PY
      exit 0
    fi
  fi
fi

if [[ ! -d "$WIKI_DIR/.git" ]]; then
  echo "::error::Wiki directory '$WIKI_DIR' is not a git repository."
  exit 1
fi

repo_head="$(git rev-parse --short HEAD 2>/dev/null || echo "local")"
timestamp_utc="$(date -u +"%Y-%m-%dT%H:%M:%SZ")"

declare -a expected_targets=()
for entry in "${mapping_entries[@]}"; do
  source_path="${entry%%|*}"
  target_path="${entry#*|}"

  if [[ -z "$source_path" || -z "$target_path" || "$source_path" == "$target_path" ]]; then
    echo "::warning::Skipping malformed wiki mapping entry '$entry'."
    continue
  fi

  if [[ ! -f "$source_path" ]]; then
    echo "::warning::Skipping missing wiki source '$source_path'."
    continue
  fi

  target_abs="$WIKI_DIR/$target_path"
  mkdir -p "$(dirname "$target_abs")"

  {
    echo "<!-- AUTO-SYNCED FILE: DO NOT EDIT IN WIKI REPO -->"
    echo "<!-- source: ${source_path} -->"
    echo "<!-- source_commit: ${repo_head} -->"
    echo "<!-- synced_at_utc: ${timestamp_utc} -->"
    echo
    cat "$source_path"
    echo
  } > "$target_abs"

  expected_targets+=("$target_path")
done

INDEX_PAGE="$WIKI_DIR/Auto-SDLC-Snapshots.md"
{
  echo "# Automated SDLC Wiki Snapshots"
  echo
  echo "This page is maintained by scripts/workflow-sync-wiki.sh."
  echo
  echo "- Source commit: ${repo_head}"
  echo "- Synced at (UTC): ${timestamp_utc}"
  echo
  echo "## Synced Pages"
  echo
  for target in "${expected_targets[@]}"; do
    echo "- [${target}](${target})"
  done
  echo
} > "$INDEX_PAGE"

git -C "$WIKI_DIR" add -A

if git -C "$WIKI_DIR" diff --cached --quiet; then
  echo "No wiki changes detected."
  WIKI_REPORT_STATUS="no-changes" \
  WIKI_REPORT_DIR="$WIKI_DIR" \
  WIKI_REPORT_RUN_ID="$RUN_ID" \
  WIKI_REPORT_RUN_ATTEMPT="$RUN_ATTEMPT" \
  python - "$WIKI_OUTPUT_PATH" <<'PY'
import json
import os
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
path.parent.mkdir(parents=True, exist_ok=True)
payload = {
    "status": os.environ.get("WIKI_REPORT_STATUS", "no-changes"),
    "wiki_dir": os.environ.get("WIKI_REPORT_DIR", ""),
    "run_id": os.environ.get("WIKI_REPORT_RUN_ID", ""),
    "run_attempt": os.environ.get("WIKI_REPORT_RUN_ATTEMPT", ""),
}
path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
PY
  exit 0
fi

declare -a changed_pages=()
while IFS= read -r changed_file; do
  changed_pages+=("$changed_file")
done < <(git -C "$WIKI_DIR" diff --cached --name-only)

if [[ "$WIKI_DRY_RUN" == "true" ]]; then
  echo "Wiki sync dry-run changes: ${changed_pages[*]}"
  status="dry-run"
  commit_sha=""
else
  git -C "$WIKI_DIR" config user.name "github-actions[bot]"
  git -C "$WIKI_DIR" config user.email "41898282+github-actions[bot]@users.noreply.github.com"
  git -C "$WIKI_DIR" commit -m "$WIKI_COMMIT_MESSAGE" >/dev/null
  commit_sha="$(git -C "$WIKI_DIR" rev-parse HEAD)"

  if [[ "$WIKI_PUSH" == "true" ]]; then
    if [[ -n "${GH_TOKEN:-}" ]]; then
      current_url="$(git -C "$WIKI_DIR" remote get-url origin 2>/dev/null || true)"
      if [[ "$current_url" == https://github.com/* ]]; then
        git -C "$WIKI_DIR" remote set-url origin "https://x-access-token:${GH_TOKEN}@${current_url#https://}"
      fi
    fi

    git -C "$WIKI_DIR" push origin HEAD >/dev/null
  fi

  status="updated"
  echo "Wiki sync committed: ${commit_sha}"
fi

CHANGED_PAGES_CSV="$(IFS=','; echo "${changed_pages[*]}")"

WIKI_STATUS="$status" \
WIKI_COMMIT_SHA="$commit_sha" \
WIKI_CHANGED_PAGES="$CHANGED_PAGES_CSV" \
WIKI_REPORT_DIR="$WIKI_DIR" \
WIKI_REPORT_RUN_ID="$RUN_ID" \
WIKI_REPORT_RUN_ATTEMPT="$RUN_ATTEMPT" \
python - "$WIKI_OUTPUT_PATH" <<'PY'
import json
import os
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
path.parent.mkdir(parents=True, exist_ok=True)
changed_csv = os.environ.get("WIKI_CHANGED_PAGES", "")
payload = {
    "status": os.environ.get("WIKI_STATUS", "unknown"),
    "commit_sha": os.environ.get("WIKI_COMMIT_SHA", ""),
    "changed_pages": [item for item in changed_csv.split(",") if item],
  "wiki_dir": os.environ.get("WIKI_REPORT_DIR", ""),
  "run_id": os.environ.get("WIKI_REPORT_RUN_ID", ""),
  "run_attempt": os.environ.get("WIKI_REPORT_RUN_ATTEMPT", ""),
}
path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
PY
