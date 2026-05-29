# War Expansion Loop Memory

## Active Loop Goals

- Keep Banana vs Bean conflict growth deterministic under policy bounds.
- Expand terrain chunk frontier under crowd pressure and cycle biome layers.
- Drive escalation tiers (peaceful/skirmish/siege) from active war population pressure.
- Keep generated-vector asset scaffolding aligned with gameplay model IDs.

## Iteration Checklist

- Add or update model-id vector profile mappings.
- Ensure runtime entities carry `gameplay_model_id` where applicable.
- Validate war pressure expansion path with focused native tests.
- Validate tier transitions and tier-based reinforcement/cooldown adjustments.
- Regenerate coherent-world evidence artifact bundle.
- Record new policy/env knobs and defaults.

## Current Runtime Anchors

- Spec: `.specify/specs/031-unified-coherent-world/spec.md`
- Integration contract: `.specify/integrations/war-expansion-loop.md`
- Evidence script: `scripts/refresh-coherent-world-evidence.sh`
