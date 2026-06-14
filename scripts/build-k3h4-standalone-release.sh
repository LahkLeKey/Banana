#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

usage() {
  cat <<'EOF'
Usage: bash scripts/build-k3h4-standalone-release.sh --version <version> [--native-lib <path>]

Builds a standalone K3H4 model release bundle under artifacts/native/k3h4/releases/<version>.

Options:
  --version     Required bundle version string (example: 2026.06.14 or v1.0.0)
  --native-lib  Optional path to libbanana_native.so. If omitted, script auto-discovers.
EOF
}

VERSION=""
NATIVE_LIB=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --version)
      VERSION="${2:-}"
      shift 2
      ;;
    --native-lib)
      NATIVE_LIB="${2:-}"
      shift 2
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "[k3h4-release] unknown option: $1" >&2
      usage
      exit 1
      ;;
  esac
done

if [[ -z "$VERSION" ]]; then
  echo "[k3h4-release] --version is required" >&2
  usage
  exit 1
fi

if [[ -z "$NATIVE_LIB" ]]; then
  CANDIDATES=(
    "$ROOT_DIR/out/v3-native/bin/libbanana_native.so"
    "$ROOT_DIR/out/native/bin/libbanana_native.so"
  )

  for candidate in "${CANDIDATES[@]}"; do
    if [[ -f "$candidate" ]]; then
      NATIVE_LIB="$candidate"
      break
    fi
  done

  if [[ -z "$NATIVE_LIB" ]]; then
    discovered="$(find "$ROOT_DIR/out" -type f -name libbanana_native.so | head -n 1 || true)"
    if [[ -n "$discovered" ]]; then
      NATIVE_LIB="$discovered"
    fi
  fi
fi

if [[ -z "$NATIVE_LIB" || ! -f "$NATIVE_LIB" ]]; then
  echo "[k3h4-release] libbanana_native.so not found; build native first or pass --native-lib" >&2
  exit 1
fi

RELEASE_ROOT="$ROOT_DIR/artifacts/native/k3h4/releases/$VERSION"
BUNDLE_DIR="$RELEASE_ROOT/k3h4-model-$VERSION"

rm -rf "$BUNDLE_DIR"
mkdir -p "$BUNDLE_DIR/bin" "$BUNDLE_DIR/include" "$BUNDLE_DIR/contracts"

cp "$NATIVE_LIB" "$BUNDLE_DIR/bin/libbanana_native.so"
cp "$ROOT_DIR/src/native/include/banana_native_v3.h" "$BUNDLE_DIR/include/banana_native_v3.h"
cp "$ROOT_DIR/.specify/specs/035-native-k3h4/contracts/native-k3h4-abi.md" "$BUNDLE_DIR/contracts/native-k3h4-abi.md"
cp "$ROOT_DIR/.specify/specs/035-native-k3h4/contracts/api-netcode-k3h4.md" "$BUNDLE_DIR/contracts/api-netcode-k3h4.md"

COMMIT_SHA="$(git -C "$ROOT_DIR" rev-parse HEAD)"
CREATED_UTC="$(date -u +"%Y-%m-%dT%H:%M:%SZ")"

cat > "$BUNDLE_DIR/MODEL_METADATA.json" <<EOF
{
  "model": "k3h4",
  "bundle_version": "$VERSION",
  "created_utc": "$CREATED_UTC",
  "git_commit": "$COMMIT_SHA",
  "native_library": "bin/libbanana_native.so",
  "abi_header": "include/banana_native_v3.h",
  "contracts": [
    "contracts/native-k3h4-abi.md",
    "contracts/api-netcode-k3h4.md"
  ],
  "standalone": true
}
EOF

cat > "$BUNDLE_DIR/README.md" <<EOF
# K3H4 Standalone Model Bundle

This bundle is a standalone packaging of the Banana K3H4 native model contract.

## Contents

- \`bin/libbanana_native.so\`
- \`include/banana_native_v3.h\`
- \`contracts/native-k3h4-abi.md\`
- \`contracts/api-netcode-k3h4.md\`
- \`MODEL_METADATA.json\`

## Quick Use

1. Extract this archive on a Linux host.
2. Set \`BANANA_NATIVE_PATH\` to the extracted \`bin/libbanana_native.so\` path.
3. Keep the ABI header and contract docs paired with this exact bundle version.
EOF

ARCHIVE_PATH="$RELEASE_ROOT/k3h4-model-$VERSION.tar.gz"
CHECKSUM_PATH="$RELEASE_ROOT/k3h4-model-$VERSION.sha256"
NOTES_PATH="$RELEASE_ROOT/RELEASE_NOTES.md"

mkdir -p "$RELEASE_ROOT"
tar -C "$RELEASE_ROOT" -czf "$ARCHIVE_PATH" "k3h4-model-$VERSION"

(
  cd "$RELEASE_ROOT"
  sha256sum "$(basename "$ARCHIVE_PATH")" > "$(basename "$CHECKSUM_PATH")"
)

cat > "$NOTES_PATH" <<EOF
# K3H4 Standalone Model Release $VERSION

This release provides a standalone K3H4 model bundle suitable for native/API runtime integration.

## Assets

- $(basename "$ARCHIVE_PATH")
- $(basename "$CHECKSUM_PATH")

## Source Commit

- $COMMIT_SHA
EOF

echo "[k3h4-release] bundle directory: $BUNDLE_DIR"
echo "[k3h4-release] archive: $ARCHIVE_PATH"
echo "[k3h4-release] checksum: $CHECKSUM_PATH"
echo "[k3h4-release] notes: $NOTES_PATH"

if [[ -n "${GITHUB_OUTPUT:-}" ]]; then
  echo "bundle_dir=$BUNDLE_DIR" >> "$GITHUB_OUTPUT"
  echo "archive_path=$ARCHIVE_PATH" >> "$GITHUB_OUTPUT"
  echo "checksum_path=$CHECKSUM_PATH" >> "$GITHUB_OUTPUT"
  echo "notes_path=$NOTES_PATH" >> "$GITHUB_OUTPUT"
fi
