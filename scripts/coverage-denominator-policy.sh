#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

usage() {
  cat <<'EOF'
Usage:
  coverage-denominator-policy.sh --snapshot-output <path> [--policy-version <version>] [--owner <owner>] [--rounding-rule <rule>]
  coverage-denominator-policy.sh --is-applicable --domain <domain> --layer <layer> [--reason-output <path>]
EOF
}

snapshot_output=""
policy_version="${BANANA_COVERAGE_POLICY_VERSION:-v1}"
owner="banana-delivery"
rounding_rule="round-half-up-to-2-decimals"
is_applicable="false"
domain=""
layer=""
reason_output=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --snapshot-output)
      snapshot_output="$2"
      shift 2
      ;;
    --policy-version)
      policy_version="$2"
      shift 2
      ;;
    --owner)
      owner="$2"
      shift 2
      ;;
    --rounding-rule)
      rounding_rule="$2"
      shift 2
      ;;
    --is-applicable)
      is_applicable="true"
      shift
      ;;
    --domain)
      domain="$2"
      shift 2
      ;;
    --layer)
      layer="$2"
      shift 2
      ;;
    --reason-output)
      reason_output="$2"
      shift 2
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "Unknown argument: $1"
      usage
      exit 2
      ;;
  esac
done

if [[ "$is_applicable" == "true" ]]; then
  if [[ -z "$domain" || -z "$layer" ]]; then
    echo "--domain and --layer are required with --is-applicable"
    exit 2
  fi

  applicable="true"
  reason="applicable"

  if [[ "$domain" == "shared-ui" && "$layer" == "e2e" ]]; then
    applicable="false"
    reason="shared-ui-e2e-not-applicable"
  elif [[ "$domain" == "native" && "$layer" == "e2e" ]]; then
    applicable="false"
    reason="native-e2e-not-applicable"
  fi

  if [[ -n "$reason_output" ]]; then
    mkdir -p "$(dirname "$reason_output")"
    printf '%s\n' "$reason" > "$reason_output"
  fi

  printf '%s\n' "$applicable"
  exit 0
fi

if [[ -z "$snapshot_output" ]]; then
  echo "--snapshot-output is required"
  usage
  exit 2
fi

mkdir -p "$(dirname "$snapshot_output")"

cat > "$snapshot_output" <<EOF
{
  "schema_version": 1,
  "policy_version": "$policy_version",
  "owner": "$owner",
  "rounding_rule": "$rounding_rule",
  "include_roots": [
    "src/native",
    "src/c-sharp/asp.net",
    "src/typescript"
  ],
  "exclude_globs": [
    "**/node_modules/**",
    "**/dist/**",
    "**/build/**",
    "**/obj/**",
    "**/bin/**",
    "**/*.g.cs",
    "**/*.generated.*"
  ],
  "not_applicable_rules": [
    {
      "domain": "shared-ui",
      "layer": "e2e",
      "reason": "shared-ui-e2e-not-applicable"
    },
    {
      "domain": "native",
      "layer": "e2e",
      "reason": "native-e2e-not-applicable"
    }
  ]
}
EOF

echo "Wrote denominator policy snapshot to $snapshot_output"
