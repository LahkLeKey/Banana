#!/usr/bin/env bash

set -e

JSON_MODE=false
DRY_RUN=false
ALLOW_EXISTING=false
SHORT_NAME=""
BRANCH_NUMBER=""
USE_TIMESTAMP=false
DESCRIPTION=""
ARGS=()
i=1
while [ $i -le $# ]; do
    arg="${!i}"
    case "$arg" in
        --json)
            JSON_MODE=true
            ;;
        --dry-run)
            DRY_RUN=true
            ;;
        --allow-existing-branch)
            ALLOW_EXISTING=true
            ;;
        --description)
            if [ $((i + 1)) -gt $# ]; then
                echo 'Error: --description requires a value' >&2
                exit 1
            fi
            i=$((i + 1))
            DESCRIPTION="${!i}"
            ;;
        --short-name)
            if [ $((i + 1)) -gt $# ]; then
                echo 'Error: --short-name requires a value' >&2
                exit 1
            fi
            i=$((i + 1))
            next_arg="${!i}"
            # Check if the next argument is another option (starts with --)
            if [[ "$next_arg" == --* ]]; then
                echo 'Error: --short-name requires a value' >&2
                exit 1
            fi
            SHORT_NAME="$next_arg"
            ;;
        --number)
            if [ $((i + 1)) -gt $# ]; then
                echo 'Error: --number requires a value' >&2
                exit 1
            fi
            i=$((i + 1))
            next_arg="${!i}"
            if [[ "$next_arg" == --* ]]; then
                echo 'Error: --number requires a value' >&2
                exit 1
            fi
            BRANCH_NUMBER="$next_arg"
            ;;
        --timestamp)
            USE_TIMESTAMP=true
            ;;
        --help|-h)
            echo "Usage: $0 [--json] [--dry-run] [--allow-existing-branch] [--short-name <name>] [--number N] [--timestamp] <feature_description>"
            echo ""
            echo "Options:"
            echo "  --json              Output in JSON format"
            echo "  --dry-run           Compute branch name and paths without creating branches, directories, or files"
            echo "  --allow-existing-branch  Switch to branch if it already exists instead of failing"
            echo "  --description <text>  Pre-populate spec sections from description (domain-aware)"
            echo "  --short-name <name> Provide a custom short name (2-4 words) for the branch"
            echo "  --number N          Specify branch number manually (overrides auto-detection)"
            echo "  --timestamp         Use timestamp prefix (YYYYMMDD-HHMMSS) instead of sequential numbering"
            echo "  --help, -h          Show this help message"
            echo ""
            echo "Examples:"
            echo "  $0 'Add user authentication system' --short-name 'user-auth'"
            echo "  $0 'Implement OAuth2 integration for API' --number 5"
            echo "  $0 --timestamp --short-name 'user-auth' 'Add user authentication'"
            exit 0
            ;;
        *)
            ARGS+=("$arg")
            ;;
    esac
    i=$((i + 1))
done

FEATURE_DESCRIPTION="${ARGS[*]}"
if [ -z "$FEATURE_DESCRIPTION" ]; then
    echo "Usage: $0 [--json] [--dry-run] [--allow-existing-branch] [--short-name <name>] [--number N] [--timestamp] <feature_description>" >&2
    exit 1
fi

# Trim whitespace and validate description is not empty (e.g., user passed only whitespace)
FEATURE_DESCRIPTION=$(echo "$FEATURE_DESCRIPTION" | xargs)
if [ -z "$FEATURE_DESCRIPTION" ]; then
    echo "Error: Feature description cannot be empty or contain only whitespace" >&2
    exit 1
fi

# Function to get highest number from specs directory
get_highest_from_specs() {
    local specs_dir="$1"
    local highest=0

    if [ -d "$specs_dir" ]; then
        for dir in "$specs_dir"/*; do
            [ -d "$dir" ] || continue
            dirname=$(basename "$dir")
            # Match sequential prefixes (>=3 digits), but skip timestamp dirs.
            if echo "$dirname" | grep -Eq '^[0-9]{3,}-' && ! echo "$dirname" | grep -Eq '^[0-9]{8}-[0-9]{6}-'; then
                number=$(echo "$dirname" | grep -Eo '^[0-9]+')
                number=$((10#$number))
                if [ "$number" -gt "$highest" ]; then
                    highest=$number
                fi
            fi
        done
    fi

    echo "$highest"
}

# Function to get highest number from git branches
get_highest_from_branches() {
    git branch -a 2>/dev/null | sed 's/^[* ]*//; s|^remotes/[^/]*/||' | _extract_highest_number
}

# Extract the highest sequential feature number from a list of ref names (one per line).
# Shared by get_highest_from_branches and get_highest_from_remote_refs.
_extract_highest_number() {
    local highest=0
    while IFS= read -r name; do
        [ -z "$name" ] && continue
        if echo "$name" | grep -Eq '^[0-9]{3,}-' && ! echo "$name" | grep -Eq '^[0-9]{8}-[0-9]{6}-'; then
            number=$(echo "$name" | grep -Eo '^[0-9]+' || echo "0")
            number=$((10#$number))
            if [ "$number" -gt "$highest" ]; then
                highest=$number
            fi
        fi
    done
    echo "$highest"
}

# Function to get highest number from remote branches without fetching (side-effect-free)
get_highest_from_remote_refs() {
    local highest=0

    for remote in $(git remote 2>/dev/null); do
        local remote_highest
        remote_highest=$(GIT_TERMINAL_PROMPT=0 git ls-remote --heads "$remote" 2>/dev/null | sed 's|.*refs/heads/||' | _extract_highest_number)
        if [ "$remote_highest" -gt "$highest" ]; then
            highest=$remote_highest
        fi
    done

    echo "$highest"
}

# Function to check existing branches (local and remote) and return next available number.
# When skip_fetch is true, queries remotes via ls-remote (read-only) instead of fetching.
check_existing_branches() {
    local specs_dir="$1"
    local skip_fetch="${2:-false}"

    if [ "$skip_fetch" = true ]; then
        # Side-effect-free: query remotes via ls-remote
        local highest_remote=$(get_highest_from_remote_refs)
        local highest_branch=$(get_highest_from_branches)
        if [ "$highest_remote" -gt "$highest_branch" ]; then
            highest_branch=$highest_remote
        fi
    else
        # Fetch all remotes to get latest branch info (suppress errors if no remotes)
        git fetch --all --prune >/dev/null 2>&1 || true
        local highest_branch=$(get_highest_from_branches)
    fi

    # Get highest number from ALL specs (not just matching short name)
    local highest_spec=$(get_highest_from_specs "$specs_dir")

    # Take the maximum of both
    local max_num=$highest_branch
    if [ "$highest_spec" -gt "$max_num" ]; then
        max_num=$highest_spec
    fi

    # Return next number
    echo $((max_num + 1))
}

# Function to clean and format a branch name
clean_branch_name() {
    local name="$1"
    echo "$name" | tr '[:upper:]' '[:lower:]' | sed 's/[^a-z0-9]/-/g' | sed 's/-\+/-/g' | sed 's/^-//' | sed 's/-$//'
}

# Resolve repository root using common.sh functions which prioritize .specify over git
SCRIPT_DIR="$(CDPATH="" cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/common.sh"

REPO_ROOT=$(get_repo_root)

# Check if git is available at this repo root (not a parent)
if has_git; then
    HAS_GIT=true
else
    HAS_GIT=false
fi

cd "$REPO_ROOT"

SPECS_DIR="$REPO_ROOT/.specify/specs"
if [ "$DRY_RUN" != true ]; then
    mkdir -p "$SPECS_DIR"
fi

# Function to generate branch name with stop word filtering and length filtering
generate_branch_name() {
    local description="$1"

    # Common stop words to filter out
    local stop_words="^(i|a|an|the|to|for|of|in|on|at|by|with|from|is|are|was|were|be|been|being|have|has|had|do|does|did|will|would|should|could|can|may|might|must|shall|this|that|these|those|my|your|our|their|want|need|add|get|set)$"

    # Convert to lowercase and split into words
    local clean_name=$(echo "$description" | tr '[:upper:]' '[:lower:]' | sed 's/[^a-z0-9]/ /g')

    # Filter words: remove stop words and words shorter than 3 chars (unless they're uppercase acronyms in original)
    local meaningful_words=()
    for word in $clean_name; do
        # Skip empty words
        [ -z "$word" ] && continue

        # Keep words that are NOT stop words AND (length >= 3 OR are potential acronyms)
        if ! echo "$word" | grep -qiE "$stop_words"; then
            if [ ${#word} -ge 3 ]; then
                meaningful_words+=("$word")
            elif echo "$description" | grep -q "\b${word^^}\b"; then
                # Keep short words if they appear as uppercase in original (likely acronyms)
                meaningful_words+=("$word")
            fi
        fi
    done

    # If we have meaningful words, use first 3-4 of them
    if [ ${#meaningful_words[@]} -gt 0 ]; then
        local max_words=3
        if [ ${#meaningful_words[@]} -eq 4 ]; then max_words=4; fi

        local result=""
        local count=0
        for word in "${meaningful_words[@]}"; do
            if [ $count -ge $max_words ]; then break; fi
            if [ -n "$result" ]; then result="$result-"; fi
            result="$result$word"
            count=$((count + 1))
        done
        echo "$result"
    else
        # Fallback to original logic if no meaningful words found
        local cleaned=$(clean_branch_name "$description")
        echo "$cleaned" | tr '-' '\n' | grep -v '^$' | head -3 | tr '\n' '-' | sed 's/-$//'
    fi
}

# Generate branch name
if [ -n "$SHORT_NAME" ]; then
    # Use provided short name, just clean it up
    BRANCH_SUFFIX=$(clean_branch_name "$SHORT_NAME")
else
    # Generate from description with smart filtering
    BRANCH_SUFFIX=$(generate_branch_name "$FEATURE_DESCRIPTION")
fi

# Warn if --number and --timestamp are both specified
if [ "$USE_TIMESTAMP" = true ] && [ -n "$BRANCH_NUMBER" ]; then
    >&2 echo "[specify] Warning: --number is ignored when --timestamp is used"
    BRANCH_NUMBER=""
fi

# Determine branch prefix
if [ "$USE_TIMESTAMP" = true ]; then
    FEATURE_NUM=$(date +%Y%m%d-%H%M%S)
    BRANCH_NAME="${FEATURE_NUM}-${BRANCH_SUFFIX}"
else
    # Determine branch number
    if [ -z "$BRANCH_NUMBER" ]; then
        if [ "$DRY_RUN" = true ] && [ "$HAS_GIT" = true ]; then
            # Dry-run: query remotes via ls-remote (side-effect-free, no fetch)
            BRANCH_NUMBER=$(check_existing_branches "$SPECS_DIR" true)
        elif [ "$DRY_RUN" = true ]; then
            # Dry-run without git: local spec dirs only
            HIGHEST=$(get_highest_from_specs "$SPECS_DIR")
            BRANCH_NUMBER=$((HIGHEST + 1))
        elif [ "$HAS_GIT" = true ]; then
            # Check existing branches on remotes
            BRANCH_NUMBER=$(check_existing_branches "$SPECS_DIR")
        else
            # Fall back to local directory check
            HIGHEST=$(get_highest_from_specs "$SPECS_DIR")
            BRANCH_NUMBER=$((HIGHEST + 1))
        fi
    fi

    # Force base-10 interpretation to prevent octal conversion (e.g., 010 → 8 in octal, but should be 10 in decimal)
    FEATURE_NUM=$(printf "%03d" "$((10#$BRANCH_NUMBER))")
    BRANCH_NAME="${FEATURE_NUM}-${BRANCH_SUFFIX}"
fi

# GitHub enforces a 244-byte limit on branch names
# Validate and truncate if necessary
MAX_BRANCH_LENGTH=244
if [ ${#BRANCH_NAME} -gt $MAX_BRANCH_LENGTH ]; then
    # Calculate how much we need to trim from suffix
    # Account for prefix length: timestamp (15) + hyphen (1) = 16, or sequential (3) + hyphen (1) = 4
    PREFIX_LENGTH=$(( ${#FEATURE_NUM} + 1 ))
    MAX_SUFFIX_LENGTH=$((MAX_BRANCH_LENGTH - PREFIX_LENGTH))

    # Truncate suffix at word boundary if possible
    TRUNCATED_SUFFIX=$(echo "$BRANCH_SUFFIX" | cut -c1-$MAX_SUFFIX_LENGTH)
    # Remove trailing hyphen if truncation created one
    TRUNCATED_SUFFIX=$(echo "$TRUNCATED_SUFFIX" | sed 's/-$//')

    ORIGINAL_BRANCH_NAME="$BRANCH_NAME"
    BRANCH_NAME="${FEATURE_NUM}-${TRUNCATED_SUFFIX}"

    >&2 echo "[specify] Warning: Branch name exceeded GitHub's 244-byte limit"
    >&2 echo "[specify] Original: $ORIGINAL_BRANCH_NAME (${#ORIGINAL_BRANCH_NAME} bytes)"
    >&2 echo "[specify] Truncated to: $BRANCH_NAME (${#BRANCH_NAME} bytes)"
fi

FEATURE_DIR="$SPECS_DIR/$BRANCH_NAME"
SPEC_FILE="$FEATURE_DIR/spec.md"

if [ "$DRY_RUN" != true ]; then
    if [ "$HAS_GIT" = true ]; then
        branch_create_error=""
        if ! branch_create_error=$(git checkout -q -b "$BRANCH_NAME" 2>&1); then
            current_branch="$(git rev-parse --abbrev-ref HEAD 2>/dev/null || true)"
            # Check if branch already exists
            if git branch --list "$BRANCH_NAME" | grep -q .; then
                if [ "$ALLOW_EXISTING" = true ]; then
                    # If we're already on the branch, continue without another checkout.
                    if [ "$current_branch" = "$BRANCH_NAME" ]; then
                        :
                    # Otherwise switch to the existing branch instead of failing.
                    elif ! switch_branch_error=$(git checkout -q "$BRANCH_NAME" 2>&1); then
                        >&2 echo "Error: Failed to switch to existing branch '$BRANCH_NAME'. Please resolve any local changes or conflicts and try again."
                        if [ -n "$switch_branch_error" ]; then
                            >&2 printf '%s\n' "$switch_branch_error"
                        fi
                        exit 1
                    fi
                elif [ "$USE_TIMESTAMP" = true ]; then
                    >&2 echo "Error: Branch '$BRANCH_NAME' already exists. Rerun to get a new timestamp or use a different --short-name."
                    exit 1
                else
                    >&2 echo "Error: Branch '$BRANCH_NAME' already exists. Please use a different feature name or specify a different number with --number."
                    exit 1
                fi
            else
                >&2 echo "Error: Failed to create git branch '$BRANCH_NAME'."
                if [ -n "$branch_create_error" ]; then
                    >&2 printf '%s\n' "$branch_create_error"
                else
                    >&2 echo "Please check your git configuration and try again."
                fi
                exit 1
            fi
        fi
    else
        >&2 echo "[specify] Warning: Git repository not detected; skipped branch creation for $BRANCH_NAME"
    fi

    mkdir -p "$FEATURE_DIR"

    if [ ! -f "$SPEC_FILE" ]; then
        TEMPLATE=$(resolve_template "spec-template" "$REPO_ROOT") || true
        if [ -n "$TEMPLATE" ] && [ -f "$TEMPLATE" ]; then
            cp "$TEMPLATE" "$SPEC_FILE"
        else
            echo "Warning: Spec template not found; created empty spec file" >&2
            touch "$SPEC_FILE"
        fi
    fi

    # Enrich spec.md with description-derived content if --description was provided (spec 104 FR-1/FR-2).
    if [ -n "$DESCRIPTION" ]; then
        python - "$SPEC_FILE" "$BRANCH_NAME" "$DESCRIPTION" "$FEATURE_NUM" <<'PY'
import sys
import re
import pathlib
import datetime

spec_path = pathlib.Path(sys.argv[1])
branch_name = sys.argv[2]
description = sys.argv[3]
feature_num = sys.argv[4]
today = datetime.date.today().isoformat()

desc_lower = description.lower()

# Domain classifier (FR-2) — keyword-driven, no external deps.
DOMAIN_MAP = {
    "native": {
        "keywords": ["native", "cmake", " c ", ".c ", "banana_", "libbanana", "wrapper", "abi", "ffi"],
        "wave": "native",
        "tech": "C, CMake",
        "deps": "libbanana_native.so, CMake presets",
        "testing": "CTest, native unit tests under tests/native/",
    },
    "api": {
        "keywords": ["api", "fastify", "asp.net", "dotnet", "endpoint", "controller", "route", "middleware", "swagger"],
        "wave": "api",
        "tech": "TypeScript/Bun or C#/.NET 8",
        "deps": "Fastify, Prisma 7, pg-boss, zod",
        "testing": "Bun test, dotnet test with coverage",
    },
    "frontend": {
        "keywords": ["react", "electron", "vite", "tailwind", "component", "ui", "frontend", "shadcn", "mobile", "expo"],
        "wave": "frontend",
        "tech": "TypeScript, React, Bun, Vite, Tailwind",
        "deps": "Bun, React, @banana/ui, Tailwind CSS",
        "testing": "Biome lint, TypeScript typecheck",
    },
    "infra": {
        "keywords": ["docker", "compose", "container", "dockerfile", "nginx", "k8s", "kubernetes", "helm", "ci", "cd"],
        "wave": "infra",
        "tech": "Bash, Docker Compose, GitHub Actions YAML",
        "deps": "Docker Compose, GitHub Actions",
        "testing": "Compose smoke tests, CI workflow validation",
    },
    "workflow": {
        "keywords": ["workflow", "action", "pipeline", "orchestrat", "automation", "speckit", "spec drain", "sdlc", "script"],
        "wave": "automation",
        "tech": "Bash, Python 3, GitHub Actions YAML",
        "deps": "gh CLI, existing orchestration scripts",
        "testing": "Bash unit tests, dry-run smoke tests",
    },
    "data": {
        "keywords": ["postgres", "prisma", "migration", "schema", "database", "sql", "corpus", "training", "model"],
        "wave": "data",
        "tech": "SQL, Prisma 7, Python 3",
        "deps": "PostgreSQL, Prisma, Python",
        "testing": "Migration tests, schema validation",
    },
    "security": {
        "keywords": ["security", "auth", "oauth", "jwt", "csp", "pentest", "sbom", "cve", "threat", "sentry", "rate limit"],
        "wave": "security",
        "tech": "Varies by security domain",
        "deps": "OWASP tooling, GitHub Dependabot",
        "testing": "Security scanning, policy checks",
    },
    "dx": {
        "keywords": ["devex", "dx", "devcontainer", "codespace", "pre-commit", "hook", "linting", "biome", "stryker"],
        "wave": "dx",
        "tech": "Bash, TypeScript, VS Code",
        "deps": "Biome, pre-commit, VS Code extensions",
        "testing": "Local validation scripts",
    },
}

detected_domain = "workflow"
for domain, info in DOMAIN_MAP.items():
    if any(kw in desc_lower for kw in info["keywords"]):
        detected_domain = domain
        break

domain_info = DOMAIN_MAP[detected_domain]

# Build a concise In Scope list from description keywords.
problem_statement = description.strip()
if not problem_statement.endswith("."):
    problem_statement += "."

# Read existing content to check if it's a fresh stub.
content = spec_path.read_text(encoding="utf-8")

# Replace known placeholder patterns.
replacements = {
    "[FEATURE NAME]": branch_name.replace("-", " ").title(),
    "[###-feature-name]": branch_name,
    "[DATE]": today,
    "**Status**: Draft": "**Status**: Stub",
    "**Input**: User description: \"$ARGUMENTS\"": f"**Wave**: {domain_info['wave']}\n**Domain**: {detected_domain}",
}
for old, new in replacements.items():
    content = content.replace(old, new)

# If Problem Statement section is placeholder/empty, inject description.
content = re.sub(
    r'(## Problem Statement\s*\n)(<!--.*?-->)(\s*)',
    lambda m: m.group(1) + f"\n{problem_statement}\n\n",
    content,
    flags=re.DOTALL,
)

spec_path.write_text(content, encoding="utf-8")

# Generate skeleton plan.md if it doesn't exist (FR-3).
plan_path = spec_path.parent / "plan.md"
if not plan_path.exists():
    plan_content = f"""# Implementation Plan: {branch_name.replace("-", " ").title()}

**Branch**: `{branch_name}` | **Date**: {today} | **Spec**: `.specify/specs/{branch_name}/spec.md`
**Input**: Feature specification from `.specify/specs/{branch_name}/spec.md`

## Summary

{problem_statement}

## Technical Context

**Language/Version**: {domain_info["tech"]}
**Primary Dependencies**: {domain_info["deps"]}
**Storage**: File-based artifacts under `artifacts/`
**Testing**: {domain_info["testing"]}
**Target Platform**: GitHub Actions + local Git Bash operator path
**Project Type**: {detected_domain.title()} — {domain_info["wave"]} wave
**Performance Goals**: Standard CI execution within job time limits
**Constraints**: Must not bypass existing safety controls or required checks
**Scale/Scope**: Scoped to spec #{feature_num} deliverables only

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- Existing safety contracts are preserved.
- No production deployment bypasses are introduced.
- All changes are auditable by persisted artifacts and PR evidence.

## Project Structure

<!-- Fill in relevant source paths for this spec -->

## Phases

### Phase 1: Setup

<!-- Define setup tasks -->

### Phase 2: Implementation

<!-- Define implementation tasks -->

### Phase 3: Validation

<!-- Define validation tasks -->
"""
    plan_path.write_text(plan_content, encoding="utf-8")
    print(f"[specify] Generated plan.md skeleton for domain: {detected_domain}", file=sys.stderr)
PY
    fi

    # Inform the user how to persist the feature variable in their own shell
    printf '# To persist: export SPECIFY_FEATURE=%q\n' "$BRANCH_NAME" >&2
fi

if $JSON_MODE; then
    if command -v jq >/dev/null 2>&1; then
        if [ "$DRY_RUN" = true ]; then
            jq -cn \
                --arg branch_name "$BRANCH_NAME" \
                --arg spec_file "$SPEC_FILE" \
                --arg feature_num "$FEATURE_NUM" \
                '{BRANCH_NAME:$branch_name,SPEC_FILE:$spec_file,FEATURE_NUM:$feature_num,DRY_RUN:true}'
        else
            jq -cn \
                --arg branch_name "$BRANCH_NAME" \
                --arg spec_file "$SPEC_FILE" \
                --arg feature_num "$FEATURE_NUM" \
                '{BRANCH_NAME:$branch_name,SPEC_FILE:$spec_file,FEATURE_NUM:$feature_num}'
        fi
    else
        if [ "$DRY_RUN" = true ]; then
            printf '{"BRANCH_NAME":"%s","SPEC_FILE":"%s","FEATURE_NUM":"%s","DRY_RUN":true}\n' "$(json_escape "$BRANCH_NAME")" "$(json_escape "$SPEC_FILE")" "$(json_escape "$FEATURE_NUM")"
        else
            printf '{"BRANCH_NAME":"%s","SPEC_FILE":"%s","FEATURE_NUM":"%s"}\n' "$(json_escape "$BRANCH_NAME")" "$(json_escape "$SPEC_FILE")" "$(json_escape "$FEATURE_NUM")"
        fi
    fi
else
    echo "BRANCH_NAME: $BRANCH_NAME"
    echo "SPEC_FILE: $SPEC_FILE"
    echo "FEATURE_NUM: $FEATURE_NUM"
    if [ "$DRY_RUN" != true ]; then
        printf '# To persist in your shell: export SPECIFY_FEATURE=%q\n' "$BRANCH_NAME"
    fi
fi
