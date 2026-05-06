# 013 ML Brain Composition & Productionization SPIKE -- Execution Tracker

Read-only SPIKE. No source, test, or ABI changes. Outputs are
implementation-ready readiness packets for slices **014**, **015**, **016**.

## Status

**SPIKE COMPLETE** -- all 9 tasks closed. See `tasks.md`.

## Recommended next-slice order

1. **014** -- ASP.NET ensemble pipeline (gated cascade)
   -> [analysis/followup-A-aspnet-ensemble.md](./analysis/followup-A-aspnet-ensemble.md)
2. **015** -- React `BananaBadge` ensemble variant
   -> [analysis/followup-B-react-ensemble-badge.md](./analysis/followup-B-react-ensemble-badge.md)
3. **016** -- Transformer quantized embedding (GATED)
   -> [analysis/followup-C-quantized-embedding.md](./analysis/followup-C-quantized-embedding.md)

## Decision artifacts

- [analysis/composition-strategy.md](./analysis/composition-strategy.md) --
  gated cascade strategy with worked anchor walks for all 6 documented
  payloads from 011/012.
- [analysis/consumer-mapping.md](./analysis/consumer-mapping.md) --
  brain-ABI to consumer-file mapping with owning helper agents.
- [analysis/deferred-registry.md](./analysis/deferred-registry.md) --
  refreshed registry: U-001 = `defer-with-trigger`, 009 pairwise edges = `drop`.

## Anchor sources cited

- [010-left-brain-regression-runnable](../010-left-brain-regression-runnable/)
- [011-right-brain-binary-runnable](../011-right-brain-binary-runnable/)
- [012-full-brain-transformer-runnable](../012-full-brain-transformer-runnable/)

## Out of scope (preserved)

- `src/native/**` is read-only in this SPIKE -- native lane stays at 7/7.
- `src/c-sharp/**` and `src/typescript/**` are read-only in this SPIKE.
- `BANANA_WRAPPER_ABI_VERSION_MAJOR=2, MINOR=2` -- unchanged.
- `.specify/feature.json` is NOT repointed by this SPIKE.

## Close-out note

The three follow-up specs (014, 015, 016) have been scaffolded as sibling
directories to this SPIKE so they can be picked up directly by the
`speckit-tasks` flow. Slice 016 is gated and SHOULD NOT be picked up until
the trigger in [analysis/deferred-registry.md](./analysis/deferred-registry.md)
fires.
