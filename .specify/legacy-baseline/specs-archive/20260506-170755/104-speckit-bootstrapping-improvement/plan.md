# Implementation Plan: Speckit Bootstrapping Improvement

**Branch**: `104-speckit-bootstrapping-improvement` | **Spec**: `.specify/specs/104-speckit-bootstrapping-improvement/spec.md`
**Input**: Feature specification from `.specify/specs/104-speckit-bootstrapping-improvement/spec.md`

## Summary

Reduce bootstrapping friction for each new spec by auto-populating `spec.md` from a description, detecting the domain, generating a `plan.md` skeleton, and adding a quality gate that blocks malformed specs from entering the drain queue.

## Technical Context

**Language/Version**: Bash, Python 3.x
**Primary Dependencies**: `.specify/scripts/bash/create-new-feature.sh`, `workflow-orchestrate-sdlc.sh`, Python `re` / `pathlib` / `datetime`
**Storage**: File-based — spec/plan/tasks under `.specify/specs/`, template under `.specify/templates/`
**Testing**: Bash smoke tests, validate-spec-quality.sh self-test
**Target Platform**: GitHub Actions + local Git Bash operator path
**Project Type**: Workflow/DX
**Performance Goals**: Bootstrapping completes in under 5 seconds locally
**Constraints**: No external deps (no LLM API, no npm packages in bash), keyword-only domain classifier
**Scale/Scope**: Scoped to spec 104 deliverables only

## Constitution Check

- Existing safety contracts are preserved — no bypasses.
- All changes are auditable by existing file-based artifact patterns.
- No production-path code is changed; only tooling scripts.

## Project Structure

```
.specify/scripts/bash/create-new-feature.sh  ← --description, domain classifier, plan stub
.specify/templates/plan-skeleton.md          ← parameterized plan template
scripts/validate-spec-quality.sh             ← quality gate script
scripts/workflow-orchestrate-sdlc.sh         ← speckit-bootstrap mode
.specify/specs/104-.../spec.md               ← this spec
.specify/specs/104-.../plan.md               ← this file
.specify/specs/104-.../tasks.md              ← tasks (sibling)
```

## Phases

### Phase 1: Quality Gate

**Goal**: Block malformed specs before they enter the drain queue.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Create `validate-spec-quality.sh` | `scripts/validate-spec-quality.sh` | Exit 0 on clean spec; exit 1 with errors on stubs |
| Integrate gate into `workflow-spec-drain-loop.sh` | `scripts/workflow-spec-drain-loop.sh` | Loop calls gate before delegating; bad spec → `mark-failed quality_gate` |

### Phase 2: Description-Driven Bootstrap

**Goal**: Pre-populate spec.md and generate plan.md skeleton when `--description` is passed.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Add `--description` arg to `create-new-feature.sh` | `.specify/scripts/bash/create-new-feature.sh` | `--description "..."` accepted without error |
| Implement domain classifier (8 buckets) | Same file | Correct domain detected from keyword match |
| Inject description into Problem Statement | Same file | `spec.md` Problem Statement section populated |
| Generate `plan.md` skeleton on creation | Same file | `plan.md` exists with domain-specific Technical Context block |
| Create plan skeleton template | `.specify/templates/plan-skeleton.md` | Template file exists with all sections |

### Phase 3: One-Command Bootstrap

**Goal**: Run `specify plan` + `specify tasks` in a single step.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Add `speckit-bootstrap` mode to `workflow-orchestrate-sdlc.sh` | `scripts/workflow-orchestrate-sdlc.sh` | `BANANA_SDLC_MODE=speckit-bootstrap` runs quality gate + specify plan + specify tasks |

### Phase 4: Validation

| Task | File(s) | Acceptance |
|------|---------|------------|
| Spec quality gate passes on spec 104 itself | `scripts/validate-spec-quality.sh` | Exit 0 |
| Dry-run bootstrap test | CLI | `--dry-run --description` produces expected branch name and plan.md |
