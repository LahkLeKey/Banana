# Implementation Plan: Wiki Contract and Mirror Restabilization

**Branch**: `105-wiki-contract-mirror-restabilization` | **Date**: 2026-05-02 | **Spec**: `.specify/specs/105-wiki-contract-mirror-restabilization/spec.md`
**Input**: Feature specification from `.specify/specs/105-wiki-contract-mirror-restabilization/spec.md`

## Summary

Restore deterministic parity between `.wiki` section indexes, `.specify/wiki/human-reference` mirrors, and allowlist contracts while ensuring CI and operator guidance clearly separate true wiki drift from transient GitHub API rate-limit failures.

## Technical Context

**Language/Version**: Bash, Python 3.x
**Primary Dependencies**: `scripts/wiki-scaffold.sh`, `scripts/wiki-consume-into-specify.sh`, `scripts/validate-ai-contracts.py`
**Storage**: Repo markdown and mirror files
**Testing**: contract validator and scaffold validation commands
**Target Platform**: GitHub Actions + local Git Bash on Windows
**Project Type**: Docs/workflow stability
**Performance Goals**: single-command drift repair within routine maintenance time
**Constraints**: preserve section-based structure and allowlist policy
**Scale/Scope**: `.wiki`, `.specify/wiki`, workflow guardrails, and related docs

## Constitution Check

- [x] Existing safety contracts are preserved.
- [x] No production deployment bypasses are introduced.
- [x] All changes are auditable by persisted artifacts and PR evidence.
- [x] GitHub/API-dependent automation has an explicit rate-limit resilience path and rerun guidance.

## Project Structure

```text
.specify/specs/105-wiki-contract-mirror-restabilization/
.wiki/
.specify/wiki/
scripts/wiki-scaffold.sh
scripts/wiki-consume-into-specify.sh
scripts/validate-ai-contracts.py
.github/workflows/wiki-lint.yml
```

## Phases

### Phase 1: Contract Parity Restoration

**Goal**: Ensure source wiki, mirror, and allowlist agree.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Regenerate section indexes | `.wiki/*/README.md`, `scripts/wiki-scaffold.sh` | `bash scripts/wiki-scaffold.sh --dry-run --validate` returns success |
| Reconcile mirror and allowlist mapping | `.specify/wiki/human-reference/**`, `.specify/wiki/human-reference-allowlist.txt` | `python scripts/validate-ai-contracts.py` returns zero issues |

### Phase 2: CI and Recovery Hardening

**Goal**: Make remediation deterministic and operator-friendly.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Keep wiki-lint guardrails explicit | `.github/workflows/wiki-lint.yml` | wiki contract checks fail only on real drift |
| Add rate-limit-aware failure guidance | spec/plan/tasks artifacts | 403 API exhaustion has a documented rerun path separate from drift remediation |

### Phase 3: Validation

**Goal**: Capture closure evidence for parity and resilience.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Run validator and scaffold checks | repo root commands | both validation commands pass |
| Record run IDs and command outputs | `tasks.md` | closure evidence can be audited in PR history |
