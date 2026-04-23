# Banana Classifier Roadmap

## Product Goal

Build a reliable classifier that answers one question consistently: is this a banana or not a banana, and expose that capability through a simple frontend flow.

## Phase 1: Classifier Specialization

1. Expand and clean the corpus in `data/not-banana/corpus.json` with representative false-positive and false-negative examples.
2. Tune training profile/session settings in `scripts/train-not-banana-model.py` for stable hold-out performance.
3. Keep generated artifacts and native inference behavior aligned through `src/native/core/domain/banana_not_banana.c`.
4. Define release acceptance thresholds (signal score, F1, drift guardrails) and keep them enforced in `.github/workflows/train-not-banana-model.yml`.

## Phase 2: Inference Contract Hardening

1. Normalize API response shape and semantics in `src/typescript/api/src/domains/not-banana/routes.ts`.
2. Keep managed/native mapping deterministic in `src/c-sharp/asp.net/NativeInterop` and pipeline behavior in `src/c-sharp/asp.net`.
3. Add focused tests for edge cases: low confidence, ambiguous inputs, and known confusing terms.

## Phase 3: Simple Frontend MVP

1. Build one focused screen in `src/typescript/react/src` for input -> classify -> result.
2. Display decision, confidence, and clear error/loading states.
3. Keep frontend model logic thin and typed; consume API output without client-local thresholds.

## Phase 4: Iteration Loop

1. Capture misclassifications from manual QA and runtime observation.
2. Add those examples to corpus curation backlog.
3. Retrain, validate drift, and republish artifacts through the existing workflow path.

## Validation Baseline

1. `python scripts/train-not-banana-model.py --corpus data/not-banana/corpus.json --output artifacts/not-banana-model`
2. `Build Native Library`
3. `Build Banana API`
4. `bun run check` and `bun run build` in `src/typescript/react` for frontend slices.
5. Expand to integration and compose validation when changes cross domains.
