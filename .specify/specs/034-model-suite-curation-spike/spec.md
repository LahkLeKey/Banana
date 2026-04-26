# Feature Specification: Model Suite Curation SPIKE

**Feature Branch**: `[034-model-suite-curation-spike]`
**Created**: 2026-04-26
**Status**: Draft -- ready to investigate
**Type**: SPIKE (investigate -> plan a small family of bounded follow-ups)
**Triggered by**: The chatbot experience we want to orchestrate
later needs more than a single not-banana classifier. We will need
a small **suite** of models -- banana / not-banana, ripeness,
escalation explainer, possibly an intent router -- each with its
own seed corpus under `data/<model>/`. Today only `data/not-banana/`
exists, and only one trainer produces it. Before we add more
models we need a curation contract so the suite stays consistent.

## Problem

Today's `data/` story is one folder, one corpus, one trainer:

```
data/
  not-banana/
    corpus.json
```

For a chatbot orchestrator we will want, at minimum:

- `data/banana/` -- positive class corpus.
- `data/not-banana/` -- negative class corpus (exists).
- `data/ripeness/` -- multi-class ripeness corpus.
- (later) `data/escalation/`, `data/intent/`, etc.

Each model needs:

- A seed corpus with a documented schema (versioned).
- A trainer entry point.
- A model image / artifact landing path.
- A managed promotion path (similar to
  `scripts/manage-not-banana-model-image.py`).
- A consumer (the API ensemble pipeline / the future chatbot
  orchestrator).

Adding models ad-hoc would diverge schemas + duplicate trainer
plumbing. We need a SPIKE to pick the curation contract before any
new model lands.

## Investigation goals

- **G1**: Inventory the existing `data/not-banana/corpus.json`
  shape, the not-banana trainer's input contract, the model image
  format produced by `manage-not-banana-model-image.py`, and how
  the API consumes it. Output: existing-model contract snapshot.
- **G2**: Pick the **initial suite** for the chatbot orchestrator:
  - Confirm: banana, not-banana, ripeness.
  - Defer: escalation explainer (depends on richer training
    signal), intent router (waits on chatbot scaffold).
- **G3**: Pick the corpus schema convention:
  - Single `corpus.json` per model with shared `schema_version`,
    `description`, `samples[]` shape.
  - Per-class subdirectories vs flat `samples[]` with `label`
    field.
  - How to mark train / eval splits.
- **G4**: Pick the trainer + model-image conventions:
  - One trainer script per model under `scripts/train-<model>.py`,
    or a parametrized `scripts/train-model.py --model <name>`.
  - Model image landing path: keep `scripts/manage-<model>-image.py`
    pattern, or generalize to `scripts/manage-model-image.py
    --model <name>`.
- **G5**: Pick the chatbot-orchestrator boundary -- what does the
  orchestrator need from each model (probability + label, embedding,
  explanation hook)?

## Out of Scope

- Implementing the new trainers themselves (slice work after this
  SPIKE picks the contract).
- Implementing the chatbot orchestrator (separate SPIKE; this SPIKE
  only defines the **suite shape** the orchestrator will consume).
- Replacing the API ensemble pipeline.
- Migrating `data/not-banana/corpus.json` to a new schema (defer
  with explicit trigger; preserve back-compat in any new schema).
- Cross-model evaluation benchmarks.

## Deliverables

- `analysis/existing-model-snapshot.md` -- G1 output.
- `analysis/initial-suite.md` -- G2 decision (banana, not-banana,
  ripeness; defer escalation + intent with explicit triggers).
- `analysis/corpus-schema.md` -- G3 decision; concrete schema
  example for `data/banana/corpus.json` and
  `data/ripeness/corpus.json`.
- `analysis/trainer-and-image-convention.md` -- G4 decision.
- `analysis/orchestrator-boundary.md` -- G5 decision (the contract
  surface each model exposes to the future chatbot orchestrator).
- `analysis/followup-readiness-banana-corpus.md` -- bounded scope
  for the slice that adds `data/banana/corpus.json` + trainer.
- `analysis/followup-readiness-ripeness-corpus.md` -- bounded
  scope for the slice that adds `data/ripeness/corpus.json` +
  trainer.

## Success Criteria

- Initial suite is explicitly limited to three models. Anything
  larger is deferred with a named trigger.
- Corpus schema is back-compat with `data/not-banana/corpus.json`
  (so the existing trainer keeps working unchanged).
- Each follow-up slice (banana corpus, ripeness corpus) is small
  enough to fit <=20 tasks.
- No code or contract changes land in this SPIKE.
- Spec/tasks parity passes.

## Validation lane

```
bash scripts/validate-spec-tasks-parity.sh --all
ls .specify/specs/034-model-suite-curation-spike/analysis/
```

## Downstream

- Slice that adds `data/banana/corpus.json` + the banana trainer.
- Slice that adds `data/ripeness/corpus.json` + the ripeness
  trainer.
- Future SPIKE: chatbot orchestrator scaffold (consumes the suite
  via the boundary defined in G5).

## In-scope files

- `.specify/specs/034-model-suite-curation-spike/**` (NEW).
- `data/**`, `scripts/train-not-banana-model.py`,
  `scripts/manage-not-banana-model-image.py`,
  `src/c-sharp/asp.net/Pipeline/**` (READ ONLY -- audit input).
- `.specify/feature.json` (repoint during execution).
