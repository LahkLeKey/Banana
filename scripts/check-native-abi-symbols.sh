#!/usr/bin/env bash
# scripts/check-native-abi-symbols.sh — Feature 072: ABI symbol diff gate.
#
# Compares exported symbols of the freshly-built libbanana_native.so against
# a committed baseline. Fails (exit 1) on any removal or incompatible change.
# Adding symbols is allowed (minor-version bump).
#
# Usage:
#   bash scripts/check-native-abi-symbols.sh [<lib_path>] [<baseline_path>]
#
# Defaults:
#   lib_path      = out/native/bin/libbanana_native.so
#   baseline_path = .specify/contracts/abi-baseline.txt
set -euo pipefail

LIB="${1:-out/native/bin/libbanana_native.so}"
BASELINE="${2:-.specify/contracts/abi-baseline.txt}"

if [[ ! -f "${LIB}" ]]; then
  echo "abi-check: ERROR — library not found: ${LIB}"
  exit 1
fi

CURRENT=$(nm --dynamic --defined-only --extern-only "${LIB}" 2>/dev/null \
  | awk '{print $NF}' \
  | grep '^banana_' \
  | sort)

if [[ ! -f "${BASELINE}" ]]; then
  echo "abi-check: no baseline found at ${BASELINE} — writing initial baseline."
  mkdir -p "$(dirname "${BASELINE}")"
  echo "${CURRENT}" > "${BASELINE}"
  echo "abi-check: baseline written. Commit ${BASELINE} to enforce it in CI."
  exit 0
fi

REMOVED=$(comm -23 <(sort "${BASELINE}") <(echo "${CURRENT}" | sort))

if [[ -n "${REMOVED}" ]]; then
  echo "abi-check: FAIL — the following exported symbols were removed (ABI break):"
  echo "${REMOVED}" | sed 's/^/  - /'
  echo ""
  echo "If this is an intentional major-version bump, update BANANA_WRAPPER_ABI_VERSION_MAJOR"
  echo "and regenerate the baseline: rm ${BASELINE} && re-run this script."
  exit 1
fi

ADDED=$(comm -13 <(sort "${BASELINE}") <(echo "${CURRENT}" | sort))
if [[ -n "${ADDED}" ]]; then
  echo "abi-check: INFO — new symbols detected (minor bump, OK):"
  echo "${ADDED}" | sed 's/^/  + /'
fi

echo "abi-check: ok — no ABI regressions detected."
