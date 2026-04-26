# Tasks: CI Training Session Persistence

**Branch**: 039-ci-training-session-persistence | **Created**: 2026-04-26

## Phase 1 -- Setup

- [x] T001 Update feature tracker docs for spec 039 in .specify/specs/039-ci-training-session-persistence/README.md.
- [x] T002 Document persisted session contract in .specify/specs/039-ci-training-session-persistence/contracts/training-session-contract.md.

## Phase 2 -- Foundational

- [x] T003 Add shared persisted-session serializer helper in scripts/train-banana-model.py.
- [x] T004 [P] Add shared persisted-session serializer helper in scripts/train-not-banana-model.py.
- [x] T005 [P] Add shared persisted-session serializer helper in scripts/train-ripeness-model.py.
- [x] T006 Add persisted session schema validator utility in scripts/check-corpus-schema.py or a dedicated scripts/validate-training-session-schema.py.

## Phase 3 -- US1 Persist CI sessions by lane (Priority: P1)

**Goal**: Persist normalized training sessions for all three lanes under data/.

**Independent Test**: Run each trainer once and verify a new append-only session record appears in its lane session folder.

- [x] T007 [US1] Persist banana training sessions under data/banana/sessions/ from scripts/train-banana-model.py.
- [x] T008 [US1] Persist not-banana training sessions under data/not-banana/sessions/ from scripts/train-not-banana-model.py.
- [x] T009 [US1] Persist ripeness training sessions under data/ripeness/sessions/ from scripts/train-ripeness-model.py.
- [x] T010 [US1] Add deterministic replay metadata fields (fingerprint/profile/thresholds) to all lane session records.

## Phase 4 -- US2 Validate CI persistence contract (Priority: P2)

**Goal**: CI fails fast if persisted session records regress.

**Independent Test**: Intentionally omit a required field in a local fixture and verify validation fails with actionable output.

- [x] T011 [US2] Add workflow step to validate persisted session schema in .github/workflows/train-not-banana-model.yml.
- [x] T012 [P] [US2] Add equivalent persisted session validation in other model-training workflow surfaces that run in CI.
- [x] T013 [US2] Ensure validation error output names missing fields and offending session file path.

## Phase 5 -- US3 Cross-environment replay readiness (Priority: P3)

**Goal**: Local runs can rehydrate and reason over CI-persisted sessions.

**Independent Test**: Run a local training command using persisted session history and verify deterministic parity signals.

- [x] T014 [US3] Add replay loader behavior in trainer scripts to consume persisted session history where applicable.
- [x] T015 [US3] Document local replay flow and required commands in .specify/specs/039-ci-training-session-persistence/quickstart.md.
- [x] T016 [US3] Add drift diagnostic summary for CI vs local session comparison in trainer output.

## Phase 6 -- Validation and close-out

- [x] T017 Run python scripts/validate-ai-contracts.py.
- [x] T018 Run bash scripts/validate-spec-tasks-parity.sh --all.
- [x] T019 Run all three training commands in ci profile and confirm session files append correctly.
- [x] T020 Mark spec 039 tasks complete and update README execution tracker.
