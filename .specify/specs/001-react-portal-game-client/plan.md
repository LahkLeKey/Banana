# Implementation Plan: Banana Engineer

**Branch**: `001-react-portal-game-client` | **Date**: 2026-05-09 | **Spec**: `.specify/specs/001-react-portal-game-client/spec.md`
**Input**: Feature specification from `.specify/specs/001-react-portal-game-client/spec.md`

## Summary

Refocus Banana toward Banana Engineer, a 2.5D game-client-style React portal experience, by rebuilding the landing route as a WASM-rendered entry surface and introducing a C-based procedural asset compiler that generates web-consumable visuals without manual art dependencies.

## Technical Context

**Language/Version**: TypeScript, React, Vite, C (native asset compiler), WASM runtime bindings, GitHub Actions YAML
**Primary Dependencies**: Bun, React app toolchain, native C toolchain, WASM build/bootstrap path, existing CI harness jobs
**Storage**: Repository files and build artifacts only
**Testing**: Existing pre-commit, turbo typecheck/lint, runtime lanes, landing bootstrap checks, deterministic asset generation checks
**Target Platform**: Web portal (desktop/mobile browser), GitHub-hosted runners
**Project Type**: Frontend rendering pivot + CI governance hardening
**Performance Goals**: Maintain stable startup/build behavior while introducing WASM landing rendering and generated asset ingestion
**Constraints**: Preserve runtime contracts (`VITE_BANANA_API_BASE_URL`), keep cross-viewport usability, enforce deterministic asset outputs, and avoid reintroducing workflow/spec sprawl
**Scale/Scope**: `src/typescript/react`, `src/native`, `.github/workflows`, `.specify/specs`

## Architecture Direction (DDD + SOLID)

- Domain layer: tile vocabulary, adjacency rules, and algorithm contracts (WFC/CA strategy interfaces).
- Application layer: deterministic pipeline orchestration and generation use-cases.
- Infrastructure layer: CLI parsing, filesystem paths, JSON emitters, and build integration adapters.
- Keep algorithm engines open for extension (new generation strategy) but closed for modification of unrelated layers.

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- Keep one canonical managed harness (`.github/workflows/banana.yml`).
- Treat warning/notice noise as QA-significant and fix forward.
- Preserve existing runtime contracts and avoid unrelated domain churn.
- Keep 2.5D + WASM rollout bounded to landing and shell presentation scope.
- Keep procedural asset generation deterministic and consumable by the web build path.
- Enforce DDD/SOLID decomposition in native procedural generation code before adding additional algorithm complexity.

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
src/native/
.github/workflows/
.specify/specs/
```

**Structure Decision**: Keep changes focused on React portal landing/shell rendering surfaces, C asset-generation tooling, and supporting CI/spec governance surfaces.

## Complexity Tracking

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| None anticipated | N/A | N/A |
