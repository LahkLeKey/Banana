# Tasks: 104-speckit-bootstrapping-improvement

## T001 — Create validate-spec-quality.sh

**Status**: ✅ Complete
**File(s)**: `scripts/validate-spec-quality.sh`
**Acceptance**: Script exists; exits 0 on clean spec; exits 1 with readable errors on placeholder-heavy stubs.

---

## T002 — Integrate quality gate into spec-drain loop

**Status**: ✅ Complete
**File(s)**: `scripts/workflow-spec-drain-loop.sh`
**Acceptance**: Loop calls `validate-spec-quality.sh --spec <path>` before delegating; failed gate marks spec as `quality_gate` failure.

---

## T003 — Add `--description` arg to create-new-feature.sh

**Status**: ✅ Complete
**File(s)**: `.specify/scripts/bash/create-new-feature.sh`
**Acceptance**: `--description "..."` is accepted; value is captured and passed to enrichment logic.

---

## T004 — Implement domain classifier (8 buckets)

**Status**: ✅ Complete
**File(s)**: `.specify/scripts/bash/create-new-feature.sh` (inline Python)
**Acceptance**: 8 keyword domains: native, api, frontend, infra, workflow, data, security, dx. Correct domain returned for representative keyword inputs.

---

## T005 — Inject description into spec.md Problem Statement

**Status**: ✅ Complete
**File(s)**: `.specify/scripts/bash/create-new-feature.sh` (post-creation Python)
**Acceptance**: After `create-new-feature.sh --description "..."`, `spec.md` Problem Statement contains the supplied text.

---

## T006 — Generate plan.md skeleton on spec creation

**Status**: ✅ Complete
**File(s)**: `.specify/scripts/bash/create-new-feature.sh` (post-creation Python)
**Acceptance**: `plan.md` is created in the spec dir with domain-specific Technical Context block populated.

---

## T007 — Create plan skeleton template

**Status**: ✅ Complete
**File(s)**: `.specify/templates/plan-skeleton.md`
**Acceptance**: Template file exists with all standard sections (Summary, Technical Context, Constitution Check, Phases).

---

## T008 — Add speckit-bootstrap mode to workflow-orchestrate-sdlc.sh

**Status**: ✅ Complete
**File(s)**: `scripts/workflow-orchestrate-sdlc.sh`
**Acceptance**: `BANANA_SDLC_MODE=speckit-bootstrap BANANA_SPECKIT_BOOTSTRAP_TARGET=<spec>` runs quality gate, `specify plan`, `specify tasks` in sequence; exits 0 on success.

---

## T009 — Validate spec 104 passes quality gate

**Status**: ✅ Complete
**File(s)**: `.specify/specs/104-speckit-bootstrapping-improvement/spec.md`
**Acceptance**: `bash scripts/validate-spec-quality.sh --spec .specify/specs/104-speckit-bootstrapping-improvement/spec.md` exits 0.
**Command**:
```bash
bash scripts/validate-spec-quality.sh --spec .specify/specs/104-speckit-bootstrapping-improvement/spec.md
```

---

## T010 — Smoke test: dry-run with --description

**Status**: ✅ Complete
**File(s)**: `.specify/scripts/bash/create-new-feature.sh`
**Acceptance**: Running `create-new-feature.sh --dry-run --description "improve api auth flow" "Test feature"` completes without error and outputs a branch name containing the detected domain.
