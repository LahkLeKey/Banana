# Tasks: Not-Banana Corpus Refresh

**Branch**: `038-not-banana-corpus-refresh` | **Created**: 2026-04-26
**Status**: Draft.

## Phase 1 -- Setup

- [x] T001 Repoint `.specify/feature.json` -> `038-not-banana-corpus-refresh`.
- [x] T002 Create branch `feature/038-not-banana-corpus-refresh` from current head.

## Phase 2 -- Corpus curation

- [x] T003 Replace low-quality/legacy generated entries in
  `data/not-banana/corpus.json` with curated samples meeting FR-038-02.
- [x] T004 Enforce no duplicate/near-duplicate sample texts and validate schema via
  `python scripts/check-corpus-schema.py data/not-banana/corpus.json`.
- [x] T005 Document curation rubric (source quality, domain coverage,
  anti-slop checks) in `analysis/corpus-curation-notes.md`.

## Phase 3 -- Deterministic training

- [x] T006 Update `scripts/train-not-banana-model.py` to guarantee deterministic
  repeated outputs for identical corpus+flags.
- [x] T007 Preserve trainer CLI surface and existing thresholds/contracts.
- [x] T008 Run deterministic double-train comparison and store evidence in
  `analysis/determinism-refresh-notes.md`.

## Phase 4 -- Image compatibility

- [x] T009 Validate `scripts/manage-not-banana-model-image.py` still supports
  refreshed trainer outputs (create/promote/status).
- [x] T010 Promote one refreshed candidate image and update pointers/index.

## Phase 5 -- Docs + close-out

- [x] T011 Add `README.md` execution tracker with local train/promote/rollback loop.
- [x] T012 Run `python scripts/validate-ai-contracts.py` and
  `bash scripts/validate-spec-tasks-parity.sh --all`.
- [x] T013 Mark tasks complete and set README status to COMPLETE.

## Out of scope

- API and frontend behavior changes.
- Multi-model orchestration policy changes.
- Cross-language retraining pipeline redesign.
