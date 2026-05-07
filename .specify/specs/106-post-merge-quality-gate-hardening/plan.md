# Implementation Plan: Post-Merge Quality Gate Hardening

**Branch**: `106-post-merge-quality-gate-hardening` | **Date**: 2026-05-02 | **Spec**: `.specify/specs/106-post-merge-quality-gate-hardening/spec.md`
**Input**: Feature specification from `.specify/specs/106-post-merge-quality-gate-hardening/spec.md`

## Summary

Harden cross-surface quality gates so bulk-delivery regressions are caught in one PR flow while introducing explicit rate-limit resilience for GitHub API and action-bootstrap paths that currently produce false-negative failures.

## Technical Context

**Language/Version**: YAML workflows, Bash scripts, Python helpers
**Primary Dependencies**: `.github/workflows/*.yml`, `scripts/compose-ci-*.sh`, `scripts/validate-ai-contracts.py`
**Storage**: `.artifacts` and workflow logs
**Testing**: GitHub Actions checks, targeted script validation
**Target Platform**: GitHub Actions + local Git Bash operator workflow
**Project Type**: CI/workflow hardening
**Performance Goals**: preserve practical PR feedback loop latency
**Constraints**: no bypass of required safety gates
**Scale/Scope**: ASP.NET, TS API, React build, AI contract validation, and check orchestration

## Constitution Check

- [x] Existing safety contracts are preserved.
- [x] No production deployment bypasses are introduced.
- [x] All changes are auditable by persisted artifacts and PR evidence.
- [x] GitHub/API-dependent automation has an explicit rate-limit resilience path and rerun guidance.

## Project Structure

```text
.specify/specs/106-post-merge-quality-gate-hardening/
.github/workflows/
scripts/
```

## Phases

### Phase 1: Baseline Gate Consolidation

**Goal**: Ensure core stabilization gates are consistent and required.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Verify ASP.NET, TS API, React, and AI contract lanes map to required PR checks | `.github/workflows/*.yml` | each lane has deterministic trigger and naming contract |
| Normalize failure summaries and remediation hints | workflow summary steps, helper scripts | contributors get actionable failure context in a single run |

### Phase 2: Rate-Limit Resilience

**Goal**: Prevent transient platform throttling from masquerading as repository regression.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Classify installation-token/API 403 failures explicitly | affected workflows and helper scripts | run output identifies rate-limit class |
| Provide deterministic recovery path | workflow docs/spec tasks | rerun windows and expected recovery behavior are documented |

### Phase 3: Validation

**Goal**: Prove hardening works on real run data.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Confirm healthy code paths pass post-hardening | PR checks for target lanes | consolidated gates pass on clean revision |
| Confirm rate-limit incidents recover without code changes | run/re-run evidence | incident closure evidence includes failed run + successful rerun IDs |
