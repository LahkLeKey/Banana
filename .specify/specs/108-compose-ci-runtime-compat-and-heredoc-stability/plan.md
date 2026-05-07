# Implementation Plan: Compose CI Runtime Compatibility and Heredoc Stability

**Branch**: `108-compose-ci-runtime-compat-and-heredoc-stability` | **Date**: 2026-05-02 | **Spec**: `.specify/specs/108-compose-ci-runtime-compat-and-heredoc-stability/spec.md`
**Input**: Feature specification from `.specify/specs/108-compose-ci-runtime-compat-and-heredoc-stability/spec.md`

## Summary

Fix deterministic Compose CI workflow breakage by restoring runtime compatibility exception handling and replacing fragile heredoc fallback blocks with syntax-safe artifact generation logic.

## Technical Context

**Language/Version**: GitHub Actions YAML, Bash
**Primary Dependencies**: `.github/workflows/compose-ci.yml`, `scripts/check-workflow-runtime-deprecations.sh`
**Storage**: `.artifacts/compose-ci`, `.artifacts/coverage`
**Testing**: Compose CI workflow run + workflow runtime compatibility check
**Target Platform**: GitHub Actions ubuntu-latest
**Project Type**: Workflow hardening
**Performance Goals**: no regression in current job duration profile
**Constraints**: preserve existing lane schema and coverage policy contracts
**Scale/Scope**: Compose CI workflow and related runtime compatibility inputs

## Constitution Check

- [x] Existing safety contracts are preserved.
- [x] No production deployment bypasses are introduced.
- [x] All changes are auditable by persisted artifacts and PR evidence.
- [x] GitHub/API-dependent automation has an explicit rate-limit resilience path and rerun guidance.

## Project Structure

```text
.specify/specs/108-compose-ci-runtime-compat-and-heredoc-stability/
.github/workflows/compose-ci.yml
.github/workflows/runtime-compatibility-exceptions.yml
scripts/check-workflow-runtime-deprecations.sh
```

## Phases

### Phase 1: Runtime Compatibility Restoration

**Goal**: Make runtime compatibility lane pass consistently.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Register/restore exception registry path for compose-ci runtime compatibility checks | `.github/workflows/runtime-compatibility-exceptions.yml`, `.github/workflows/compose-ci.yml` | runtime compatibility lane no longer reports `exceptions_registry_present=false` |

### Phase 2: Heredoc Safety Repair

**Goal**: Eliminate shell parsing defects in fallback artifact steps.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Rewrite fallback artifact generation to avoid malformed heredoc termination | `.github/workflows/compose-ci.yml` or extracted scripts | no `wanted EOF` / `unexpected end of file` failures |
| Add a syntax guard for fallback shell snippets | workflow or script-level validation | malformed multiline shell blocks are caught before merge |

### Phase 3: Validation

**Goal**: Confirm Compose CI health for targeted lanes.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Run Compose CI on branch and verify target lane outcomes | workflow run evidence | runtime compatibility + dotnet coverage lanes pass without syntax faults |
