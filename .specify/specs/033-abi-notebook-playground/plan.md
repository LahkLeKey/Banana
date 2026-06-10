# Implementation Plan: Notebook MMO Gameplay Client

**Branch**: `033-abi-notebook-playground` | **Date**: 2026-06-08 | **Spec**: `.specify/specs/033-abi-notebook-playground/spec.md`

**Input**: Feature specification from `.specify/specs/033-abi-notebook-playground/spec.md`

## Summary

Promote notebooks from artifact-only output to primary gameplay-client runtime UX by introducing a main-menu-first shell, decomposing the current bloated playground page into shared components, and defining API orchestration hooks with static notebook fallback.

## Technical Context

**Language/Version**: TypeScript + React (existing Vite/Bun app), Python 3 (artifact generation), Bash (workflow orchestration)

**Primary Dependencies**: React, react-router-dom, existing local fetch/browser APIs, existing notebook scaffold scripts

**Storage**: File artifacts in `notebooks/`, `src/typescript/react/public/notebooks/`, `artifacts/notebooks/`

**Testing**: React build + integrated browser validation + notebook generation command validation

**Target Platform**: Windows dev + Vercel static deployment + browser runtime

**Project Type**: Web gameplay client shell with generated-data ingestion

**Performance Goals**: Interactive file filtering/selection stays responsive for 291+ indexed files

**Constraints**: Preserve existing routes/contracts; maintain fallback when API orchestration is unavailable

**Scale/Scope**: Refactor notebook client surface into shared components and menu orchestration boundary

## Constitution Check

*GATE: Must pass before implementation.*

- [x] Player trust and disclosure alignment verified (no storefront claim changes in this phase).
- [x] Storefront governance artifacts not required for this internal gameplay client pivot.
- [x] Cross-domain contracts mapped (scripts + React shell + future API layer).
- [x] Quality gates defined (build, browser runtime behavior, notebook asset loading).
- [x] Reproducible delivery path identified (scaffold script publishes all notebook endpoints).

## Project Structure

### Documentation (feature)

```text
.specify/specs/033-abi-notebook-playground/
├── spec.md
├── plan.md
├── tasks.md
├── quickstart.md
└── contracts/
    └── notebook-client-orchestration.md
```

### Source Code (repository root)

```text
scripts/
├── scaffold-abi-notebook-workflow.sh
└── export-native-c-to-notebook.py

notebooks/
├── catalog-index.json
└── native-c-catalog.ipynb

src/typescript/react/src/
├── pages/
│   └── DataSciencePlaygroundPage.tsx
├── components/
│   └── notebook-client/           # planned extraction target
└── lib/
    └── notebook-client/           # planned orchestration hooks/models
```

**Structure Decision**: Keep deployment simple while reducing page bloat via shared notebook-client components and typed orchestration hooks.

## Delivery Notes

- Notebook artifacts are the gameplay renderer payload for this phase.
- Main menu UX is treated as gameplay client shell, not marketing landing.
- API orchestration is introduced behind typed adapters with local file fallback.

## Complexity Tracking

No constitution exceptions required.
