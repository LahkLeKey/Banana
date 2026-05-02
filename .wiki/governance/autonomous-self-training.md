<!-- breadcrumb: Governance > Autonomous Self-Training -->

# Autonomous Self-Training

> [Home](../Home.md) › [Governance](README.md) › Autonomous Self-Training

Banana's models can now keep themselves fresh on their own — both whenever
something interesting changes in the repo and on a "sleep" schedule overnight.
This page is the human-friendly view; the technical contract lives at
`.specify/wiki/human-reference/autonomous-self-training.md`.

## Two Ways The Models Retrain Themselves

1. **Something changed** — when a PR merges to `main` or a push touches a
   model's training data or trainer script, a workflow kicks in and refreshes
   only the models that are actually affected. If nothing relevant changed,
   nothing runs.
2. **Sleep schedule** — every night around 06:30 UTC the models do a light
   "NREM" consolidation pass; on Sundays around 07:30 UTC they do a deeper
   "REM" pass. The names are borrowed from neuroscience because the work
   they do (replay + memory consolidation) is loosely modeled on what a
   sleeping brain does.

## What The Neuroscience Layer Adds

- **Episodic replay** — keeps a small rolling memory (max 2048 samples) of
  past training sessions so old patterns aren't forgotten when new ones arrive.
- **Surprise weighting** — focuses extra attention on examples the model
  finds confusing, like a student practicing the harder problems first.
- **Reward modulation** — when accuracy is improving, the layer turns the
  learning rate up; when it's getting worse, it turns it down. Always
  clamped between 0.25× and 4× so it can't run away.
- **Forgetting guard** — at the end of each run, current accuracy is
  compared to a stored "anchor" snapshot. If the model regressed by more
  than 1.0 percentage point, the workflow fails so a human can look.

## How To Turn It On / Off

The whole layer is opt-in. Each trainer takes `--neuro-profile`:

- `off` (default) — totally inert, outputs are byte-for-byte identical
  to before this feature shipped.
- `ci` — observation-only sidecar, safe for CI.
- `local` — same as `ci` with longer windows for hands-on tuning.

To roll back, drop the flag and the cycle returns to its prior behavior.

## How The Workflows Decide What To Retrain

If a PR is labeled `train:not-banana` (or `train:all`), only those models
retrain. If there's no label, the workflow inspects which files changed:
touching `data/banana/` retrains the banana model, touching
`scripts/neuro/` retrains all three. If nothing relevant changed, the matrix
is skipped silently.

## Where The Artifacts Go

Everything new lands under `artifacts/training/<model>/`:

- `local/neuro-trace.json` — what the neuro layer observed during training
- `replay/buffer.jsonl` — the rolling memory
- `consolidation/<date>/fisher.json` — the nightly/weekly snapshot
- `evidence-<trigger>.json` — the bundle attached to follow-up PRs

For the deeper details, the spec is at
`.specify/specs/050-neuro-self-training-enhancements/spec.md`.
