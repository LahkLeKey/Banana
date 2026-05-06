# Implementation Plan: Web Analytics Surface Scan (256)

## Overview

Add a deterministic analytics scanner that inspects the React route tree and known interaction surfaces, then emits coverage artifacts and optional strict gate failure.

## Key Files

- `scripts/scan-web-analytics-surfaces.ts` (new)
- `src/typescript/react/src/lib/router.tsx`
- `src/typescript/react/src/lib/analytics.ts`
- `.github/workflows/playwright-reinforcement.yml` (optional gate integration later)

## Steps

1. Build route extraction logic and map route paths to analytics page-view markers.
2. Build interaction-surface checks for required analytics events.
3. Emit JSON report and Markdown summary under artifacts.
4. Add `--strict` mode with configurable thresholds.
5. Wire scanner into CI workflow after baseline calibration.

## Validation

1. Scanner output is stable across repeated local runs.
2. Introduced route or key interaction without analytics causes strict mode failure.
3. Report includes actionable missing-surface guidance.
