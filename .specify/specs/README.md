# Spec Root Baseline (Fresh)

This directory now contains the active baseline specs only.

## Active Baseline

- 050-multiplayer-server-authority

## Archived During Refresh

Superseded and out-of-scope spec folders were moved to:

.specify/legacy-baseline/specs-archive/2026-05-spec-refresh

Additional multiplayer baseline consolidation archives were moved to:

.specify/legacy-baseline/specs-archive/2026-05-multiplayer-baseline-reset

Additional non-core artifacts from active specs were archived to:

.specify/legacy-baseline/specs-archive/2026-05-spec-refresh-active-prune

## Operating Rule

Use Specify CLI flow for all new work:

1. specify specify
2. specify plan
3. specify tasks

Keep feature pointer in .specify/feature.json aligned to the currently executing spec.

## Minimal Active Spec Contract

Each active spec folder should default to these core files only:

- spec.md
- plan.md
- tasks.md
- heartbeat-log.md

Optional drilldown documents are allowed only when actively referenced by spec.md.

## Multiplayer Baseline Rule

For multiplayer execution, Spec 050 is the sole active authority. Prior foundations may remain in archive for lineage, but agents should bootstrap from 050 first and treat archived specs as historical evidence only.
