#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

RUN_ID="${GITHUB_RUN_ID:-local-run}"
RUN_ATTEMPT="${GITHUB_RUN_ATTEMPT:-1}"
WIKI_DIR="${BANANA_WIKI_DIR:-artifacts/wiki-sync/wiki-worktree}"
WIKI_REPOSITORY="${BANANA_WIKI_REPOSITORY:-${GITHUB_REPOSITORY:-}}"
WIKI_REMOTE_URL="${BANANA_WIKI_REMOTE_URL:-}"
CANONICAL_WIKI_REMOTE_URL="${BANANA_WIKI_CANONICAL_REMOTE_URL:-https://github.com/LahkLeKey/Banana.wiki.git}"
ENFORCE_CANONICAL_WIKI_REMOTE="${BANANA_ENFORCE_CANONICAL_WIKI_REMOTE:-true}"
WIKI_COMMIT_MESSAGE="${BANANA_WIKI_COMMIT_MESSAGE:-docs(wiki): sync automated SDLC documentation snapshots}"
WIKI_PUSH="${BANANA_WIKI_PUSH:-true}"
WIKI_DRY_RUN="${BANANA_WIKI_DRY_RUN:-false}"
WIKI_STRICT="${BANANA_WIKI_STRICT:-false}"
WIKI_MAPPING_RAW="${BANANA_WIKI_SYNC_MAPPING:-}"
WIKI_OUTPUT_PATH="${BANANA_WIKI_OUTPUT_PATH:-artifacts/wiki-sync/report-${RUN_ID}-attempt-${RUN_ATTEMPT}.json}"
SPECIFY_WIKI_CANONICAL_DIR="${BANANA_SPECIFY_WIKI_CANONICAL_DIR:-.specify/wiki/human-reference}"

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
    ".specify/wiki/human-reference/api-parity-governance.md|api-parity-governance.md"
    "data/not-banana/README.md|Auto-Not-Banana-Data.md"
    "data/not-banana/feedback/README.md|Auto-Not-Banana-Feedback-Inbox.md"
  )

  if [[ -d ".github/prompts" ]]; then
    while IFS= read -r prompt_path; do
      prompt_file="$(basename "$prompt_path")"
      prompt_name="${prompt_file%.prompt.md}"
      mapping_entries+=("${prompt_path}|Auto-Prompts/${prompt_name}.md")
    done < <(find .github/prompts -maxdepth 1 -type f -name "*.prompt.md" | sort)
  fi

  if [[ -f ".github/copilot-instructions.md" ]]; then
    mapping_entries+=(".github/copilot-instructions.md|Auto-GitHub/copilot-instructions.md")
  fi

  if [[ -d ".github/workflows" ]]; then
    while IFS= read -r workflow_path; do
      workflow_file="$(basename "$workflow_path")"
      mapping_entries+=("${workflow_path}|Auto-GitHub-Workflows/${workflow_file}.md")
    done < <(find .github/workflows -maxdepth 1 -type f -name "*.yml" | sort)
  fi

  if [[ -d ".github/instructions" ]]; then
    while IFS= read -r instruction_path; do
      instruction_file="$(basename "$instruction_path")"
      mapping_entries+=("${instruction_path}|Auto-GitHub-Instructions/${instruction_file}")
    done < <(find .github/instructions -maxdepth 1 -type f -name "*.md" | sort)
  fi

  # Sync all allowlisted human-reference wiki pages to the GitHub wiki
  # using their canonical relative paths (e.g. operations/deployment.md).
  if [[ -f ".specify/wiki/human-reference-allowlist.txt" && -d ".specify/wiki/human-reference" ]]; then
    while IFS= read -r raw_line; do
      line="${raw_line//$'\r'/}"
      line="$(echo "$line" | xargs)"
      [[ -z "$line" || "$line" == \#* ]] && continue
      src_rel="${line#.wiki/}"
      src_path=".specify/wiki/human-reference/${src_rel}"
      if [[ -f "$src_path" ]]; then
        mapping_entries+=("${src_path}|${src_rel}")
      fi
    done < ".specify/wiki/human-reference-allowlist.txt"
  fi
fi

if [[ -z "$WIKI_REMOTE_URL" ]]; then
  WIKI_REMOTE_URL="$CANONICAL_WIKI_REMOTE_URL"
fi

if [[ "$ENFORCE_CANONICAL_WIKI_REMOTE" == "true" && "$WIKI_REMOTE_URL" != "$CANONICAL_WIKI_REMOTE_URL" ]]; then
  if [[ "$WIKI_STRICT" == "true" ]]; then
    echo "::error::Configured wiki remote '$WIKI_REMOTE_URL' does not match canonical remote '$CANONICAL_WIKI_REMOTE_URL'."
    exit 1
  fi

  echo "::warning::Configured wiki remote '$WIKI_REMOTE_URL' does not match canonical remote '$CANONICAL_WIKI_REMOTE_URL'. Using canonical remote."
  WIKI_REMOTE_URL="$CANONICAL_WIKI_REMOTE_URL"
fi

if [[ "$WIKI_DIR" == ".wiki" ]]; then
  echo "::error::Refusing to use repository .wiki as sync worktree. Set BANANA_WIKI_DIR to an isolated path."
  exit 1
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

extract_repo_slug_from_url() {
  local remote_url="$1"
  local normalized="${remote_url%.git}"

  if [[ "$normalized" =~ ^https://github\.com/([^/]+/[^/]+)$ ]]; then
    printf '%s' "${BASH_REMATCH[1]}"
    return 0
  fi

  if [[ "$normalized" =~ ^git@github\.com:([^/]+/[^/]+)$ ]]; then
    printf '%s' "${BASH_REMATCH[1]}"
    return 0
  fi

  if [[ "$normalized" =~ ^ssh://git@github\.com/([^/]+/[^/]+)$ ]]; then
    printf '%s' "${BASH_REMATCH[1]}"
    return 0
  fi

  return 1
}

if [[ -z "$WIKI_REPOSITORY" ]] && command -v gh >/dev/null 2>&1; then
  if repo_name="$(gh repo view --json nameWithOwner -q .nameWithOwner 2>/dev/null)"; then
    WIKI_REPOSITORY="$repo_name"
  fi
fi

if [[ -z "$WIKI_REPOSITORY" ]]; then
  origin_url="$(git config --get remote.origin.url 2>/dev/null || true)"
  if [[ -n "$origin_url" ]]; then
    if repo_name="$(extract_repo_slug_from_url "$origin_url")"; then
      WIKI_REPOSITORY="$repo_name"
    fi
  fi
fi

if [[ ! -d "$WIKI_DIR/.git" ]]; then
  if [[ -z "$WIKI_REMOTE_URL" ]]; then
    if [[ -z "$WIKI_REPOSITORY" ]]; then
      if [[ "$WIKI_STRICT" == "true" ]]; then
        echo "::error::Cannot determine wiki repository slug. Set BANANA_WIKI_REPOSITORY or BANANA_WIKI_REMOTE_URL (canonical: $CANONICAL_WIKI_REMOTE_URL)."
        exit 1
      fi

      echo "::warning::Cannot determine wiki repository slug. Skipping wiki sync because strict mode is disabled."
      python - "$WIKI_OUTPUT_PATH" <<'PY'
import json
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
path.parent.mkdir(parents=True, exist_ok=True)
path.write_text(json.dumps({"status": "skipped", "reason": "wiki repository unresolved"}, indent=2) + "\n", encoding="utf-8")
PY
      exit 0
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

declare -a sorted_expected_targets=()
if [[ ${#expected_targets[@]} -gt 0 ]]; then
  mapfile -t sorted_expected_targets < <(printf '%s\n' "${expected_targets[@]}" | awk 'NF && !seen[$0]++' | sort)
fi

INDEX_PAGE="$WIKI_DIR/Auto-SDLC-Snapshots.md"
{
  echo "# Automated SDLC Wiki Snapshots"
  echo
  echo "This page is maintained by scripts/workflow-sync-wiki.sh."
  echo "Canonical AI-consumable wiki content is stored in ${SPECIFY_WIKI_CANONICAL_DIR} in the main repository."
  echo "The wiki is a human-reference publication surface."
  echo
  echo "- Source commit: ${repo_head}"
  echo "- Synced at (UTC): ${timestamp_utc}"
  echo
  echo "## Synced Pages"
  echo
  for target in "${sorted_expected_targets[@]}"; do
    echo "- [${target}](${target})"
  done
  echo
} > "$INDEX_PAGE"

AI_AUDIT_PAGE="$WIKI_DIR/AI-Audit-Trails.md"
{
  echo "# AI Audit Trails"
  echo
  echo "This section indexes pages generated or synced by automation for traceability and diagnostics."
  echo "Canonical AI-consumable wiki content is stored in ${SPECIFY_WIKI_CANONICAL_DIR} in the main repository."
  echo "Treat this wiki as a human-reference publication layer."
  echo
  echo "- Source commit: ${repo_head}"
  echo "- Synced at (UTC): ${timestamp_utc}"
  echo
  echo "## Primary Audit Pages"
  echo
  echo "- [Auto-SDLC-Snapshots.md](Auto-SDLC-Snapshots.md)"
  echo
  echo "## Auto-Synced Source Mirrors"
  echo
  if [[ ${#sorted_expected_targets[@]} -eq 0 ]]; then
    echo "- No auto-synced pages were generated in this run."
  else
    for target in "${sorted_expected_targets[@]}"; do
      echo "- [${target}](${target})"
    done
  fi
  echo
} > "$AI_AUDIT_PAGE"

declare -a human_pages=()
while IFS= read -r wiki_file; do
  rel_path="${wiki_file#"$WIKI_DIR/"}"
  if [[ "$rel_path" == .git/* ]]; then
    continue
  fi

  if [[ "$rel_path" == Auto-* ]]; then
    continue
  fi

  if [[ "$rel_path" == "AI-Audit-Trails.md" || "$rel_path" == "Human-Reading-Guide.md" ]]; then
    continue
  fi

  human_pages+=("$rel_path")
done < <(find "$WIKI_DIR" -type f -name "*.md" | sort)

declare -a sorted_human_pages=()
if [[ ${#human_pages[@]} -gt 0 ]]; then
  mapfile -t sorted_human_pages < <(printf '%s\n' "${human_pages[@]}" | awk 'NF && !seen[$0]++' | sort)
fi

HUMAN_GUIDE_PAGE="$WIKI_DIR/Human-Reading-Guide.md"
{
  echo "# Human Reading Guide"
  echo
  echo "This section highlights human-oriented wiki pages and operational references."
  echo "Canonical AI-consumable wiki content is stored in ${SPECIFY_WIKI_CANONICAL_DIR} in the main repository."
  echo "Use this wiki as human reference only."
  echo
  echo "## Start Here"
  echo
  echo "- [Home.md](Home.md)"
  echo "- [Architecture-Diagrams.md](Architecture-Diagrams.md)"
  echo "- [First-Day-Checklist.md](First-Day-Checklist.md)"
  echo
  echo "## Human-Curated Pages"
  echo
  if [[ ${#sorted_human_pages[@]} -eq 0 ]]; then
    echo "- No human-curated pages detected yet."
  else
    for page in "${sorted_human_pages[@]}"; do
      echo "- [${page}](${page})"
    done
  fi
  echo
  echo "## AI Audit Trails"
  echo
  echo "- [AI-Audit-Trails.md](AI-Audit-Trails.md)"
  echo "- [Auto-SDLC-Snapshots.md](Auto-SDLC-Snapshots.md)"
  echo
} > "$HUMAN_GUIDE_PAGE"

HOME_PAGE="$WIKI_DIR/Home.md"
python - "$HOME_PAGE" "$repo_head" "$timestamp_utc" "$SPECIFY_WIKI_CANONICAL_DIR" <<'PY'
import pathlib
import re
import sys

home_path = pathlib.Path(sys.argv[1])
source_commit = sys.argv[2]
synced_at = sys.argv[3]
canonical_wiki_dir = sys.argv[4]

start_marker = "<!-- AUTO-SYNC-WIKI-NAV START -->"
end_marker = "<!-- AUTO-SYNC-WIKI-NAV END -->"

nav_block = "\n".join(
    [
        start_marker,
        "## Wiki Navigation",
        "",
        "### Human Reading",
        "- [Human-Reading-Guide.md](Human-Reading-Guide.md)",
        "",
        "### AI Audit Trails",
        "- [AI-Audit-Trails.md](AI-Audit-Trails.md)",
        "- [Auto-SDLC-Snapshots.md](Auto-SDLC-Snapshots.md)",
        "",
        f"- Canonical AI wiki source: {canonical_wiki_dir}",
        "- Wiki role: human-reference publication",
        "",
        f"- Source commit: {source_commit}",
        f"- Synced at (UTC): {synced_at}",
        end_marker,
    ]
)

if home_path.exists():
    content = home_path.read_text(encoding="utf-8")
else:
    content = "# Banana Wiki\n"

pattern = re.compile(
    re.escape(start_marker) + r".*?" + re.escape(end_marker),
    flags=re.DOTALL,
)

if pattern.search(content):
    updated = pattern.sub(nav_block, content)
else:
    if not content.endswith("\n"):
        content += "\n"
    updated = content + "\n" + nav_block + "\n"

home_path.write_text(updated, encoding="utf-8")
PY

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
  push_status="dry-run"
else
  git -C "$WIKI_DIR" config user.name "github-actions[bot]"
  git -C "$WIKI_DIR" config user.email "41898282+github-actions[bot]@users.noreply.github.com"
  git -C "$WIKI_DIR" commit -m "$WIKI_COMMIT_MESSAGE" >/dev/null
  commit_sha="$(git -C "$WIKI_DIR" rev-parse HEAD)"

  push_status="not-requested"

  if [[ "$WIKI_PUSH" == "true" ]]; then
    if [[ -n "${GH_TOKEN:-}" ]]; then
      current_url="$(git -C "$WIKI_DIR" remote get-url origin 2>/dev/null || true)"
      if [[ "$current_url" == https://github.com/* ]]; then
        git -C "$WIKI_DIR" remote set-url origin "https://x-access-token:${GH_TOKEN}@${current_url#https://}"
      fi
    fi

    if git -C "$WIKI_DIR" push origin HEAD >/dev/null 2>&1; then
      push_status="pushed"
    else
      push_status="push-failed"
      if [[ "$WIKI_STRICT" == "true" ]]; then
        echo "::error::Failed to push wiki updates."
        exit 1
      fi

      echo "::warning::Failed to push wiki updates. Keeping local wiki commit only."
    fi
  fi

  if [[ "$push_status" == "push-failed" ]]; then
    status="updated-no-push"
  else
    status="updated"
  fi

  echo "Wiki sync committed: ${commit_sha}"
fi

CHANGED_PAGES_CSV="$(IFS=','; echo "${changed_pages[*]}")"

WIKI_STATUS="$status" \
WIKI_COMMIT_SHA="$commit_sha" \
WIKI_CHANGED_PAGES="$CHANGED_PAGES_CSV" \
WIKI_PUSH_STATUS="$push_status" \
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
  "push_status": os.environ.get("WIKI_PUSH_STATUS", ""),
    "changed_pages": [item for item in changed_csv.split(",") if item],
  "wiki_dir": os.environ.get("WIKI_REPORT_DIR", ""),
  "run_id": os.environ.get("WIKI_REPORT_RUN_ID", ""),
  "run_attempt": os.environ.get("WIKI_REPORT_RUN_ATTEMPT", ""),
}
path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
PY
