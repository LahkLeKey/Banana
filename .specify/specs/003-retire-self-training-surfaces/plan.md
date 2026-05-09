# Implementation Plan: Retire Self-Training Surfaces

**Branch**: `003-retire-self-training-surfaces` | **Date**: 2026-05-09 | **Spec**: `.specify/specs/003-retire-self-training-surfaces/spec.md`
**Input**: Feature specification from `.specify/specs/003-retire-self-training-surfaces/spec.md`

## Summary

Retire autonomous self-training from active repository surfaces by removing runtime dependencies, updating validators, and rewriting active docs to legacy/retired posture while preserving historical context only in `.specify/legacy-baseline/`.

## Technical Context

**Language/Version**: Bash, Python 3.x, YAML, Markdown
**Primary Dependencies**: GitHub Actions, repository orchestration scripts, Spec Kit validation scripts
**Storage**: File-based repository metadata and docs
**Testing**: Existing CI lanes, focused script validation, grep-based surface checks
**Target Platform**: GitHub-hosted Linux runners and local Windows/Git Bash maintenance workflows
**Project Type**: Monorepo workflow and governance refactor
**Performance Goals**: No increase in CI lane count and no new orchestration overhead
**Constraints**: Keep changes scoped to active roots; do not rewrite archival trees
**Scale/Scope**: `.github/workflows`, `scripts`, `.specify/scripts`, `docs`, `.specify/wiki`

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- Preserve one canonical managed harness in `.github/workflows/banana.yml`.
- Treat warnings/notices as QA-relevant noise to remove where feasible.
- Keep archival history under `.specify/legacy-baseline/` untouched except for explicit references from active surfaces.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/003-retire-self-training-surfaces/
├── plan.md
├── spec.md
└── tasks.md
```

### Source Code (repository root)

```text
.github/
├── skills/
└── workflows/

scripts/
.specify/
├── scripts/
└── wiki/

docs/
└── automation/agent-pulse/
```

**Structure Decision**: Restrict implementation to active workflow/script/doc roots and avoid direct edits in archived historical baselines.

## Complexity Tracking

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| None anticipated | N/A | N/A |
