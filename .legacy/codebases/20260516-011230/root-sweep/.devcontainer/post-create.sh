#!/usr/bin/env bash
set -euo pipefail

echo "[post-create] Installing Bun..."
curl -fsSL https://bun.sh/install | bash
export PATH="$HOME/.bun/bin:$PATH"

echo "[post-create] Installing shared UI deps..."
cd /workspace/src/typescript/shared/ui && bun install

echo "[post-create] Installing TS API deps..."
cd /workspace/src/typescript/api && bun install

echo "[post-create] Installing React deps..."
cd /workspace/src/typescript/react && bun install

echo "[post-create] Building native library..."
cd /workspace
cmake --preset linux-x86_64-release && \
  cmake --build build/native/linux-x86_64-release --config Release || \
  echo "[post-create] WARNING: Native build failed (optional step); continuing."

echo "[post-create] Dev container setup complete."
