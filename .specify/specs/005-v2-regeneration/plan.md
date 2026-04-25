# Implementation Plan: v2 Regeneration (Parent)

**Branch**: `005-v2-regeneration` | **Date**: 2026-04-25 | **Spec**: [spec.md](./spec.md)  
**Input**: Feature specification at `.specify/specs/005-v2-regeneration/spec.md`

## Summary

Decompose the entire Banana repo into 9 Spec Kit domain slices plus a parent
orchestrator. Each slice owns its v2 target spec, plan, tasks, quickstart, and
contracts, paired with a frozen `.specify/legacy-baseline/<domain>.md` snapshot
of v1. Implementation of each slice happens in dedicated feature branches
gated by their own plans; this parent only coordinates.

## Technical Context

**Language/Version**: Multi — C (C11), C# (.NET 8), TypeScript (Bun + Node), Bash, YAML.  
**Primary Dependencies**: CMake, ASP.NET Core, Fastify, Prisma 7, Vite, React, Electron, Expo, Docker Compose, GitHub Actions, Spec Kit.  
**Storage**: PostgreSQL (gated by `BANANA_PG_CONNECTION` / `DATABASE_URL`).  
**Testing**: xUnit (.NET), C unit tests, Bun test runner, Cobertura coverage.  
**Target Platform**: Windows host + Docker Desktop + Ubuntu WSL2 (`Ubuntu-24.04` or `Ubuntu`); Linux containers; macOS for iOS Simulator only.  
**Project Type**: Multi-language monorepo with native interop and containerized runtime channels.  
**Performance Goals**: Maintain v1 build/test wall-clock; do not regress.  
**Constraints**: Preserve hard contracts listed in each child spec; keep wiki freeze (`004-trim-vibe-drift`).  
**Scale/Scope**: 9 domain slices, ~25 native modules, ~10 controllers, 4 frontends, ~10 GH workflows, ~60 scripts.

## Constitution Check

- **Helper-agent decomposition**: PASS — each child spec maps to a narrow helper agent.
- **Wiki freeze contract**: PASS — this scaffolding does not touch `.wiki/` content.
- **AI contract guard**: PASS — validator must remain green after scaffolding.
- **Runtime contracts**: PASS — child specs preserve env vars, exit codes, scripts of record.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/005-v2-regeneration/
├── plan.md                    # this file
├── spec.md                    # parent feature spec
├── tasks.md                   # cross-slice tracking
├── quickstart.md              # how to navigate the regeneration
└── contracts/
    └── regeneration-orchestration.md
.specify/legacy-baseline/
├── README.md
├── native-core.md
├── aspnet-pipeline.md
├── typescript-api.md
├── react-shared-ui.md
├── electron-desktop.md
├── react-native-mobile.md
├── compose-runtime.md
├── ai-orchestration.md
└── test-coverage.md
.specify/specs/006-native-core/ … 014-test-coverage/
└── (full speckit set per slice)
```

### Source Code (repository root)

The parent spec produces no runtime code. Source layout per slice is captured
in each child spec's plan.

**Structure Decision**: Parent + per-domain children with paired baselines.
Coordination happens via the dependency table in `spec.md`. Implementation
PRs reference a single child slice at a time.

## Phasing

- **Phase 0 — Scaffold (this PR)**: create parent + 9 baselines + 9 child slices. No runtime code changes.
- **Phase 1 — Per-slice plans/tasks**: refine each child's `plan.md` and `tasks.md` as the slice is picked up.
- **Phase 2 — Per-slice regeneration**: implement v2 per slice on dedicated feature branches; legacy paths stay live until cutover.
- **Phase 3 — Cutover**: swap consumers to v2 per dependency edge in topological order; remove legacy when no consumer remains.

## Complexity Tracking

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|--------------------------------------|
| 10 spec directories | One per domain enables parallel agent work and per-slice cutover | Single mega-spec rejected: blocks parallel work and obscures cross-domain edges |
| Legacy baseline mirror | v1 facts must be stable while v2 designs land | Inlining baselines into child specs rejected: bloats target specs and mixes "is" with "should" |
