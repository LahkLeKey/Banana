# Implementation Plan: Pre-commit Ruff Format Stability

**Branch**: `110-precommit-ruff-format-stability` | **Date**: 2026-05-02 | **Spec**: `.specify/specs/110-precommit-ruff-format-stability/spec.md`
**Input**: Feature specification from `.specify/specs/110-precommit-ruff-format-stability/spec.md`

## Summary

Eliminate deterministic pre-commit failures caused by CI-time `ruff-format` rewrites by normalizing Python formatting drift and adding earlier drift detection in automation and contributor workflows.

## Technical Context

**Language/Version**: Python tooling, pre-commit hooks, GitHub Actions
**Primary Dependencies**: `.pre-commit-config.yaml`, Python helper scripts, pre-commit workflow
**Storage**: Git working tree + CI logs
**Testing**: `pre-commit run --all-files`, pre-commit workflow on GitHub Actions
**Target Platform**: local Git Bash + GitHub Actions
**Project Type**: Tooling/workflow stability
**Performance Goals**: keep pre-commit runtime within current envelope
**Constraints**: preserve existing hook coverage and security checks
**Scale/Scope**: Python script formatting + pre-commit execution path

## Constitution Check

- [x] Existing safety contracts are preserved.
- [x] No production deployment bypasses are introduced.
- [x] All changes are auditable by persisted artifacts and PR evidence.
- [x] GitHub/API-dependent automation has an explicit rate-limit resilience path and rerun guidance.

## Project Structure

```text
.specify/specs/110-precommit-ruff-format-stability/
.pre-commit-config.yaml
scripts/*.py
.github/workflows/pre-commit.yml
```

## Phases

### Phase 1: Drift Normalization

**Goal**: Remove currently failing format drift.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Format offending Python files touched by CI failures | `scripts/*.py` (targeted) | `pre-commit run --all-files` no longer rewrites those files |

### Phase 2: Early Drift Guardrail

**Goal**: Catch formatter drift before CI failure.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Add/confirm deterministic local + automation formatting path | scripts/workflow docs or pre-commit hook wrappers | contributors/automation can run one command to normalize drift |

### Phase 3: Validation

**Goal**: Confirm pre-commit lane stability.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Run pre-commit locally and in CI evidence path | run evidence | no `files were modified by this hook` failure on main-forward commits |
