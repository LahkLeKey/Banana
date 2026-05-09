# Implementation Plan: React Portal 2.5D Game Client Pivot

**Branch**: `001-react-portal-game-client` | **Date**: 2026-05-09 | **Spec**: `.specify/specs/001-react-portal-game-client/spec.md`
**Input**: Feature specification from `.specify/specs/001-react-portal-game-client/spec.md`

## Summary

Refocus Banana toward a 2.5D game-client-style React portal by rebuilding the landing route as a WASM-rendered entry surface while preserving deployability and reducing active scope drift from legacy research-era surfaces.

## Technical Context

**Language/Version**: TypeScript, React, Vite, WASM runtime bindings, GitHub Actions YAML
**Primary Dependencies**: Bun, React app toolchain, WASM build/bootstrap path, existing CI harness jobs
**Storage**: Repository files and build artifacts only
**Testing**: Existing pre-commit, turbo typecheck/lint, screenshot/runtime lanes, landing bootstrap checks
**Target Platform**: Web portal (desktop/mobile browser), GitHub-hosted runners
**Project Type**: Frontend rendering pivot + CI governance hardening
**Performance Goals**: Maintain stable startup/build behavior while introducing WASM landing rendering
**Constraints**: Preserve runtime contracts (`VITE_BANANA_API_BASE_URL`), keep cross-viewport usability, and avoid reintroducing workflow/spec sprawl
**Scale/Scope**: `src/typescript/react`, `.github/workflows`, `.specify/specs`

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- Keep one canonical managed harness (`.github/workflows/banana.yml`).
- Treat warning/notice noise as QA-significant and fix forward.
- Preserve existing runtime contracts and avoid unrelated domain churn.
- Keep 2.5D + WASM rollout bounded to landing and shell presentation scope.

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
src/typescript/shared/ui/
.github/workflows/
.specify/specs/
```

**Structure Decision**: Keep changes focused on React portal landing/shell rendering surfaces and supporting CI/spec governance surfaces.

## Complexity Tracking

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| None anticipated | N/A | N/A |
