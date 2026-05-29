# War Expansion Loop Memory

## Active Loop Goals

- Keep Banana vs Bean conflict growth deterministic under policy bounds.
- Expand terrain chunk frontier under crowd pressure and cycle biome layers.
- Drive escalation tiers (peaceful/skirmish/siege) from active war population pressure.
- Progress war intelligence stages and unlock all biome layers under sustained pressure.
- Keep generated-vector asset scaffolding aligned with gameplay model IDs.

## Iteration Checklist

- Add or update model-id vector profile mappings.
- Ensure runtime entities carry `gameplay_model_id` where applicable.
- Validate war pressure expansion path with focused native tests.
- Validate tier transitions and tier-based reinforcement/cooldown adjustments.
- Validate intelligence-stage progression and all-biome unlock progression.
- Validate deterministic full-biome reinforcement model family cycle (tropical/glacier/urban/volcanic) under elevated intelligence.
- Validate banana comeback-bonus behavior when bean pressure outnumbers banana pressure.
- Validate intelligence-stage apex reinforcement scaffolding (`banana-phalanx-*`, `bean-colossus-*`) and vector profile resolution.
- Validate intelligence-stage mythic reinforcement scaffolding (`banana-archon-*`, `bean-leviathan-*`) and vector profile resolution.
- Validate apex/mythic reinforcement telemetry channels (banana + bean) report deterministic per-biome totals.
- Validate stage-indexed apex/mythic telemetry rollups (banana + bean) report deterministic stage-channel totals.
- Validate Game-of-Life lane ticks produce deterministic life-generation/alive/frontline telemetry and contribute bounded intelligence-stage progression.
- Validate procedural biome bias uses life-lane outputs under policy control while preserving bounded biome rotation and unlock guarantees.
- Validate sentient humanoid scaffolding telemetry (`humanoid index`, `coordination`, `empathy`), deterministic behavior-mode classification (`hold-line`/`flank`/`regroup`/`negotiate`), mode-dependent tactical reinforcement placement geometry, and comeback-bonus integration remain bounded and deterministic.
- Validate sentience-mode skirmish asset variants (`banana-scout-*` / `bean-raider-*` flank/regroup/envoy) and per-faction mode-channel telemetry rollups (H/F/R/N) stay deterministic and visible to DX12 HUD/UI components.
- Validate scene-browser variant `6` (`banana-war-sentience-showcase`) remains launchable and pre-seeds flank/regroup/envoy placements for immediate DX12 sentience asset preview.
- Validate negotiate-mode consecutive-tick de-escalation trim behavior: streak increases while negotiation holds, trim remains bounded, spawn pressure is reduced deterministically, and streak/trim reset when negotiation breaks.
- Validate overcrowd terrain burst expansion behavior reaches additional chunk rings while preserving biome rotation bounds.
- Validate HUD/API war status visibility for overcrowd policy, burst bonus, intelligence burst policy, frontier depth, biome stage, apex activation, and mythic activation.
- Validate long-run soak invariants: monotonic frontier growth, bounded frontier cap, monotonic biome unlock progression, and full-layer unlock under sustained pressure.
- Regenerate coherent-world evidence artifact bundle.
- Record new policy/env knobs and defaults (`BANANA_CONTROLLER_WAR_OVERCROWD_INTELLIGENCE_BONUS_PER_STAGE`, future stage-gated growth controls).

## Current Runtime Anchors

- Spec: `.specify/specs/031-unified-coherent-world/spec.md`
- Integration contract: `.specify/integrations/war-expansion-loop.md`
- Evidence script: `scripts/refresh-coherent-world-evidence.sh`
