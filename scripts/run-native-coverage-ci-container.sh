#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE_TAG="${BANANA_NATIVE_COVERAGE_CI_IMAGE:-banana-native-coverage-ci:local}"
DOCKERFILE_PATH="${BANANA_NATIVE_COVERAGE_CI_DOCKERFILE:-docker/native-coverage-ci.Dockerfile}"
BUILD_IMAGE=1

coverage_args=("$@")
if [[ ${#coverage_args[@]} -eq 0 ]]; then
  coverage_args=(--build-dir out/native-coverage-ci-docker --output-dir artifacts/native/coverage-ci-docker)
fi

while [[ ${#coverage_args[@]} -gt 0 ]]; do
  case "${coverage_args[0]}" in
    --no-build-image)
      BUILD_IMAGE=0
      coverage_args=("${coverage_args[@]:1}")
      ;;
    *)
      break
      ;;
  esac
done

MOUNT_ROOT="$ROOT_DIR"
if command -v cygpath >/dev/null 2>&1; then
  MOUNT_ROOT="$(cygpath -m "$ROOT_DIR")"
fi

if [[ "$BUILD_IMAGE" -eq 1 ]]; then
  echo "[coverage-ci-container] building image $IMAGE_TAG"
  docker build \
    --file "$ROOT_DIR/$DOCKERFILE_PATH" \
    --tag "$IMAGE_TAG" \
    "$ROOT_DIR/docker"
fi

echo "[coverage-ci-container] running native coverage in CI-parity container"
MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' docker run --rm \
  --volume "$MOUNT_ROOT:/workspace" \
  --workdir /workspace \
  --env BANANA_NATIVE_ROOT_DIR=/workspace \
  "$IMAGE_TAG" \
  bash -lc 'tmp_script=/tmp/run-native-coverage.sh; tr -d "\r" < scripts/run-native-coverage.sh > "$tmp_script"; chmod +x "$tmp_script"; "$tmp_script" "$@"' _ "${coverage_args[@]}"
