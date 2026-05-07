# Feature Specification: Performance and CDN Spike (119)

**Feature Branch**: `feature/119-performance-cdn-spike`
**Created**: 2026-05-02
**Status**: Spike — research and go/no-go only
**Input**: `banana.engineer` will be a public-facing product. Identify the minimum set of performance and CDN optimizations needed to achieve acceptable Core Web Vitals on Vercel's Edge Network.
## Problem Statement

Feature Specification: Performance and CDN Spike (119) aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope *(mandatory)*

- Measure baseline LCP, FID/INP, CLS, and TTFB from the current `bun run build` output.
- Evaluate Vercel's Edge Network caching behavior for the React SPA assets.
- Identify Vite bundle split opportunities (lazy route loading, vendor chunk extraction).
- Identify font and icon loading strategies that eliminate render-blocking resources.
- Evaluate `Cache-Control` header strategy for `dist/assets/` (already in `vercel.json` baseline).
- Evaluate Vercel Image Optimization for any user-supplied image URLs in the classifier flow.
- Document findings and generate a concrete path to specs 133–136.

## Out of Scope *(mandatory)*

- Server-side rendering or Next.js migration.
- React Native or Electron performance work.
- Backend API performance (covered in spike 117 hosting evaluation).
- Observability tooling (covered by spike 120).

## User Scenarios & Testing *(mandatory)*

### User Story 1 — banana.engineer loads in under 2 seconds on a mobile connection (Priority: P1)

**Acceptance Scenarios**:

1. **Given** the production build, **When** measured with Lighthouse on a throttled mobile connection, **Then** LCP is under 2.5 s and CLS is under 0.1.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: Route-level code splitting MUST be evaluated and a recommendation documented.
- **FR-002**: `Cache-Control: immutable` MUST be confirmed working for hashed assets.
- **FR-003**: A Lighthouse baseline score (mobile) MUST be recorded in `research.md`.

## Success Criteria *(mandatory)*

### Validation Lane

```bash
python scripts/validate-spec-tasks-parity.py .specify/specs/119-performance-cdn-spike
```

## Assumptions

- Vercel's built-in CDN is the delivery mechanism; no additional CDN (Cloudflare, Fastly) is in scope for this spike.
- Lighthouse CI can be run locally or via a GitHub Actions job against the preview URL.
