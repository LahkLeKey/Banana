#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

SPECKIT_STRICT="${BANANA_SPECKIT_STRICT:-true}"
SPECKIT_VERSION="${BANANA_SPECKIT_VERSION:-v0.8.0}"

required_paths=(
  ".specify/init-options.json"
  ".specify/templates/spec-template.md"
  ".specify/scripts/bash/check-prerequisites.sh"
  ".specify/memory/constitution.md"
  ".github/skills/speckit-specify/SKILL.md"
  ".github/skills/speckit-plan/SKILL.md"
  ".github/skills/speckit-tasks/SKILL.md"
  ".github/skills/speckit-implement/SKILL.md"
)

declare -a missing_paths=()
for required_path in "${required_paths[@]}"; do
  if [[ ! -e "$required_path" ]]; then
    missing_paths+=("$required_path")
  fi
done

constitution_path=".specify/memory/constitution.md"
constitution_placeholder_detected="false"
if [[ -f "$constitution_path" ]]; then
  if grep -q "\[PROJECT_NAME\]" "$constitution_path" || grep -q "\[CONSTITUTION_VERSION\]" "$constitution_path"; then
    constitution_placeholder_detected="true"
  fi
fi

if [[ ${#missing_paths[@]} -gt 0 || "$constitution_placeholder_detected" == "true" ]]; then
  echo "::group::Spec Kit preflight"
  if [[ ${#missing_paths[@]} -gt 0 ]]; then
    echo "Missing required Spec Kit paths:"
    for missing_path in "${missing_paths[@]}"; do
      echo "- ${missing_path}"
    done
  fi

  if [[ "$constitution_placeholder_detected" == "true" ]]; then
    echo "Spec Kit constitution still contains template placeholders in ${constitution_path}."
  fi

  echo "Expected initialization command:"
  echo "  python -m uv tool run --from git+https://github.com/github/spec-kit.git@${SPECKIT_VERSION} specify init --here --ai copilot --ai-skills"
  echo "::endgroup::"

  if [[ "$SPECKIT_STRICT" == "true" ]]; then
    echo "::error::Spec Kit preflight failed. Set BANANA_SPECKIT_STRICT=false to continue without enforcement."
    exit 1
  fi

  echo "::warning::Spec Kit preflight failed but strict mode is disabled."
fi

if [[ "${BANANA_SPECKIT_VERIFY_CLI:-false}" == "true" ]]; then
  python_command=""
  if command -v python >/dev/null 2>&1; then
    python_command="python"
  elif command -v python3 >/dev/null 2>&1; then
    python_command="python3"
  fi

  if [[ -n "$python_command" ]]; then
    if ! "$python_command" -m uv tool run --from "git+https://github.com/github/spec-kit.git@${SPECKIT_VERSION}" specify version >/dev/null 2>&1; then
      echo "::warning::Unable to execute Spec Kit CLI verification command."
    fi
  else
    echo "::warning::Python runtime not found; skipping Spec Kit CLI verification."
  fi
fi
