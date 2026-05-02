#!/usr/bin/env bash
# Generates CycloneDX SBOMs for all Banana artifact surfaces.
# Requires: syft (https://github.com/anchore/syft)
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUTDIR="$ROOT/artifacts/sboms"
mkdir -p "$OUTDIR"
echo "Generating SBOMs..."
syft dir:"$ROOT/src/typescript/api" --output cyclonedx-json="$OUTDIR/ts-api.json"
syft dir:"$ROOT/src/c-sharp/asp.net" --output cyclonedx-json="$OUTDIR/dotnet-api.json"
echo "SBOMs written to $OUTDIR"
