# Tasks: Banana Corpus + Trainer

**Branch**: `035-banana-corpus-and-trainer` | **Created**: 2026-04-26
**Status**: Draft. Hard prerequisite: SPIKE 034.

## Phase 1 -- Setup

- [ ] T001 Repoint `.specify/feature.json` -> `035-banana-corpus-and-trainer`.
- [ ] T002 Create branch `feature/035-banana-corpus-and-trainer`
  from current head.

## Phase 2 -- Corpus

- [ ] T003 Author `data/banana/corpus.json` with >=12 banana +
  >=6 not-banana samples conforming to the SPIKE 034 schema.
- [ ] T004 Add a corpus-schema lint check that fails when
  `data/<model>/corpus.json` violates the SPIKE 034 contract
  (or document deferral in `analysis/schema-lint-deferral.md`).

## Phase 3 -- Trainer

- [ ] T005 Add `scripts/train-banana-model.py` mirroring the
  not-banana CLI surface (FR-035-03).
- [ ] T006 Ensure determinism: seed RNG, sort vocabulary
  emission, normalize timestamp / metadata fields. Add
  `analysis/determinism-notes.md` listing every non-deterministic
  source eliminated.
- [ ] T007 Run the trainer twice with the same flags; assert
  byte-identical output (FR-035-04).

## Phase 4 -- Image management

- [ ] T008 Add `scripts/manage-banana-image.py` mirroring
  `manage-not-banana-model-image.py` (promote, list,
  current-channel pointer).
- [ ] T009 Promote the first banana model image to
  `data/banana/images/<sha>.tar` + channel pointer JSON; commit.

## Phase 5 -- Docs + close-out

- [ ] T010 Author `README.md` execution tracker (run command, how
  to commit a new model image, how to roll back).
- [ ] T011 Author `analysis/local-training-runbook.md` -- step by
  step "run the trainer locally and commit the deterministic
  improvement".
- [ ] T012 Run `python scripts/validate-ai-contracts.py` and
  `bash scripts/validate-spec-tasks-parity.sh --all`.
- [ ] T013 Mark all tasks `[x]` and update README to COMPLETE.

## Out of scope

- Wiring banana into the API ensemble pipeline.
- Image-based classification.
- Cross-model evaluation.
