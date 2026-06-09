# Implementation Plan: ABI Notebook Playground Pivot

**Branch**: `033-abi-notebook-playground` | **Date**: 2026-06-08 | **Spec**: .specify/specs/033-abi-notebook-playground/spec.md

**Input**: Feature specification from `.specify/specs/033-abi-notebook-playground/spec.md`

## Summary

Create a first-class ABI notebook workflow that snapshots native C sources into a generated Jupyter notebook for interactive prototyping, and pivot the Vercel-served React root experience from marketing-first messaging to a data-science playground launch surface.

## Technical Context

**Language/Version**: C (existing native sources), TypeScript React (existing app), Python 3 for notebook generation tooling, Bash for orchestration

**Primary Dependencies**: Existing Bun + Vite frontend stack, standard Python json/pathlib/argparse libraries, existing native source tree

**Storage**: File-based generated artifacts under `artifacts/notebooks`

**Testing**: Native build + focused CTest smoke, React smoke import, script dry-run generation validation

**Target Platform**: Windows dev shell + Git Bash, CI/Linux-compatible shell scripts, Vercel static deployment

**Project Type**: Monorepo with native runtime, React frontend, and orchestration scripts

**Performance Goals**: Notebook generation under 30s for current native tree; generated notebook size bounded by per-file truncation

**Constraints**: Preserve existing runtime/API contracts, keep Bun contract in React app, avoid requiring notebook kernel setup to generate artifacts

**Scale/Scope**: Native C and header files under `src/native`; one generated consolidated notebook artifact for exploratory workflows

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- [x] Player trust and disclosure alignment verified (this pivot is temporary website messaging scope; no Steam claim mutation required).
- [x] Storefront governance artifacts identified when public Steam copy is affected (not affected in this feature).
- [x] Cross-domain contracts mapped for touched layers (native sources read-only ingestion, script workflow, React landing route).
- [x] Quality gates defined with measurable checks (build, targeted tests, script generation, frontend smoke import).
- [x] Reproducible delivery path identified (script-generated notebook in tracked artifacts path and Vercel-exposed notebooks route).

## Project Structure

### Documentation (this feature)

```text
.specify/specs/033-abi-notebook-playground/
├── spec.md
├── plan.md
├── tasks.md
└── contracts/
```

### Source Code (repository root)

```text
scripts/
├── scaffold-abi-notebook-workflow.sh
└── export-native-c-to-notebook.py

artifacts/
└── notebooks/

src/typescript/react/src/
├── lib/router.tsx
└── pages/DataSciencePlaygroundPage.tsx
```

**Structure Decision**: Keep implementation minimal and additive: one shell orchestrator, one Python generator, one new React page, one router pivot.

## Complexity Tracking

No constitution violations requiring exceptions.
