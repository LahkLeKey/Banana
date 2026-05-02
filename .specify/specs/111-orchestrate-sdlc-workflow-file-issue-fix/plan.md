# Implementation Plan: Orchestrate SDLC Workflow File Issue Fix

**Branch**: `111-orchestrate-sdlc-workflow-file-issue-fix` | **Date**: 2026-05-02 | **Spec**: `.specify/specs/111-orchestrate-sdlc-workflow-file-issue-fix/spec.md`
**Input**: Feature specification from `.specify/specs/111-orchestrate-sdlc-workflow-file-issue-fix/spec.md`

## Summary

Stabilize the `orchestrate-banana-sdlc` workflow definition by isolating the push-time `workflow file issue` trigger failure and applying a minimal workflow-definition fix with pre-merge validation coverage.

## Technical Context

**Language/Version**: GitHub Actions YAML + expression syntax
**Primary Dependencies**: `.github/workflows/orchestrate-banana-sdlc.yml`, workflow validation tooling
**Storage**: workflow run metadata and logs
**Testing**: workflow lint/validation + push/dispatch run evidence
**Target Platform**: GitHub Actions
**Project Type**: Workflow definition hardening
**Performance Goals**: no measurable orchestration runtime regression
**Constraints**: preserve current workflow dispatch/schedule semantics
**Scale/Scope**: orchestration workflow definition and validation gate

## Constitution Check

- [x] Existing safety contracts are preserved.
- [x] No production deployment bypasses are introduced.
- [x] All changes are auditable by persisted artifacts and PR evidence.
- [x] GitHub/API-dependent automation has an explicit rate-limit resilience path and rerun guidance.

## Project Structure

```text
.specify/specs/111-orchestrate-sdlc-workflow-file-issue-fix/
.github/workflows/orchestrate-banana-sdlc.yml
scripts/check-workflow-runtime-deprecations.sh
.github/workflows/runtime-compatibility-exceptions.yml
```

## Phases

### Phase 1: Root-Cause Isolation

**Goal**: Determine exact workflow-definition failure mechanism.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Reproduce and isolate definition issue from run metadata/validation tools | workflow + run metadata | identified parser/validator defect with reproducible signal |

### Phase 2: Workflow Definition Repair

**Goal**: Remove push-time workflow file issue while preserving behavior.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Apply minimal YAML/expression/trigger fix | `.github/workflows/orchestrate-banana-sdlc.yml` | push no longer emits workflow-file issue failure |
| Add/extend pre-merge workflow validation gate | workflow/script validation path | future definition regressions fail before merge |

### Phase 3: Validation

**Goal**: Prove workflow health across trigger modes.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Validate push + workflow_dispatch behavior | run evidence | no workflow file issue; orchestration workflow remains operational |
