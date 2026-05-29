# 031 Migration Checklist (Superseded Slices 015-030)

Use this checklist when onboarding or migrating references from legacy cohesion-lane slices. Do not add new behavior in the superseded slices; extend 031 instead.

## Rule Zero

- [ ] Confirm the change belongs to the coherent-world surface and is owned by 031.
- [ ] Confirm no edits are made to superseded spec requirements except superseded notices.

## Superseded-to-031 Mapping

| Superseded Slice | Primary 031 Anchor | Primary Verification Signal |
| --- | --- | --- |
| 015-coherent-game-world | FR-001 through FR-006 (catalog + dx12 + tick/camera/move-target ownership) | Umbrella filter + full native baseline |
| 016-gameplay-api-continuity | US1 + FR-002 + FR-003 + FR-008 + API Unit Test Baseline | Transition continuity tests + full-MMO demo + API unit baseline |
| 017-coherent-world-unification | Superseded Specs table + Umbrella Test Filter | Umbrella filter |
| 018-coherent-world-fusion | FR-001 through FR-006 ownership model | Umbrella filter + full native baseline |
| 019-coherent-world-synthesis | FR-001 through FR-006 ownership model | Umbrella filter + full native baseline |
| 020-coherent-world-convergence | FR-001 through FR-006 ownership model | Umbrella filter + full native baseline |
| 021-coherent-world-cohesion | Umbrella Test Filter + Evidence section | Umbrella baseline file |
| 022-objective-completion-cohesion | FR-004 + FR-005 | banana_dx12_objective_policy_test + lane tests |
| 023-checkpoint-id-cohesion | FR-002 + FR-003 continuity ownership | transition_continuity/roundtrip tests |
| 024-route-signature-cohesion | FR-002 + FR-003 route/transition ownership | transition_drift + continuity tests |
| 025-fusion-lane-cohesion | FR-005 lane ownership | gameplay lane/click lane tests |
| 026-synthesis-pass-cohesion | FR-006 tick phasing ownership | runtime_tick_* tests |
| 027-replay-phase-cohesion | FR-006 tick phasing ownership | runtime_tick_* + runtime_engine_tick tests |
| 028-convergence-lane-cohesion | FR-001 through FR-006 + umbrella subset | Umbrella filter |
| 029-reconciliation-lane-cohesion | FR-001 through FR-006 + full native baseline | Full native baseline |
| 030-observation-lane-cohesion | FR-001 through FR-006 + evidence discipline | Full native baseline + umbrella baseline |

## Migration Steps

- [ ] Identify legacy slice references in docs, tasks, prompts, and runbooks.
- [ ] Replace each reference with 031 section language (US/FR/Validation/Evidence anchors).
- [ ] Ensure tests are mapped to umbrella subset and, when appropriate, full native baseline.
- [ ] Ensure evidence paths point to artifacts/native/031-unified-coherent-world.
- [ ] Record migration updates in execution-dashboard and 031 tasks.

## Completion Criteria

- [ ] No active instructions tell contributors to extend 015-030.
- [ ] All coherent-world changes route to 031 tasks and evidence.
- [ ] Validation commands in docs match the canonical 031 filter and full baseline.
