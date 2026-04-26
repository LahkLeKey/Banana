# Tasks: Model Suite Curation SPIKE

**Branch**: `034-model-suite-curation-spike` | **Created**: 2026-04-26
**Status**: Draft.

## Phase 1 -- Setup

- [x] T001 Repoint `.specify/feature.json` -> `034-model-suite-curation-spike`.

## Phase 2 -- Investigation

- [x] T002 Author `analysis/existing-model-snapshot.md` -- corpus
  shape, trainer input, model-image format, API consumer path.
- [x] T003 Author `analysis/initial-suite.md` -- banana,
  not-banana, ripeness; defer escalation + intent with triggers.
- [x] T004 Author `analysis/corpus-schema.md` -- schema convention
  + concrete examples for banana + ripeness; back-compat with the
  existing not-banana corpus.
- [x] T005 Author `analysis/trainer-and-image-convention.md` --
  per-model script vs parametrized; model-image manage pattern.
- [x] T006 Author `analysis/orchestrator-boundary.md` -- per-model
  contract (label + probability + embedding + explanation hook).

## Phase 3 -- Follow-up readiness

- [x] T007 Author `analysis/followup-readiness-banana-corpus.md`.
- [x] T008 Author
  `analysis/followup-readiness-ripeness-corpus.md`.

## Phase 4 -- Close-out

- [x] T009 Run `bash scripts/validate-spec-tasks-parity.sh --all`.
- [x] T010 Mark all tasks `[x]` and update README to COMPLETE.

## Out of scope

- Implementing new trainers / corpora (downstream slices).
- Implementing the chatbot orchestrator (separate SPIKE).
- Replacing the API ensemble pipeline.
- Migrating the existing not-banana corpus to a new schema.
- Cross-model evaluation benchmarks.
