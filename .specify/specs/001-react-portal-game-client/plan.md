# Implementation Plan: React Portal Game Client Pivot

**Branch**: `001-react-portal-game-client` | **Date**: 2026-05-09 | **Spec**: `.specify/specs/001-react-portal-game-client/spec.md`
**Input**: Feature specification from `.specify/specs/001-react-portal-game-client/spec.md`

## Summary

Refocus Banana toward a game-client-style React portal while preserving deployability and reducing active scope drift from legacy research-era surfaces.

## Technical Context

**Language/Version**: TypeScript, React, Vite, GitHub Actions YAML
**Primary Dependencies**: Bun, React app toolchain, existing CI harness jobs
**Storage**: Repository files and build artifacts only
**Testing**: Existing pre-commit, turbo typecheck/lint, screenshot and runtime lanes
**Target Platform**: Web portal (desktop/mobile browser), GitHub-hosted runners
**Project Type**: Frontend pivot + CI governance hardening
**Performance Goals**: Keep existing build/runtime timings stable while preserving quality gates
**Constraints**: Preserve runtime contracts (`VITE_BANANA_API_BASE_URL`) and avoid reintroducing workflow/spec sprawl
**Scale/Scope**: `src/typescript/react`, `.github/workflows`, `.specify/specs`

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- Keep one canonical managed harness (`.github/workflows/banana.yml`).
- Treat warning/notice noise as QA-significant and fix forward.
- Preserve existing runtime contracts and avoid unrelated domain churn.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/001-react-portal-game-client/
├── plan.md
├── spec.md
└── tasks.md
```

### Source Code (repository root)

```text
src/typescript/react/
.github/workflows/
.specify/specs/
```

**Structure Decision**: Keep changes focused on React portal UX and supporting CI/spec governance surfaces.

## Complexity Tracking

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| None anticipated | N/A | N/A |
