# 031 Done Definition Checklist

Use this checklist before marking coherent-world work complete.

Daily operator runbook: `daily-pr-checklist.md`

## Spec and Ownership

- [ ] The change is described in 031 (not in a new cohesion-lane numbered spec).
- [ ] Owning source paths remain single-path for the affected concern.
- [ ] Any new field/behavior is attached to existing owning contracts.

## Tests

- [ ] Umbrella subset passes:
  - ctest --test-dir out/v3-native -C Debug --output-on-failure -R "banana_(runtime_full_mmo_demo|runtime_demo_scene_catalog_.+|dx12_(objective_policy|scene_flow|scene_overlay_frame|projection_policy|gameplay_lane|gameplay_click_lane)|runtime_tick_(input_phase|post_phase|budget_policy)|runtime_engine_tick|runtime_camera_follow_policy|runtime_move_target_domain)_test"
- [ ] Full native baseline passes:
  - ctest --test-dir out/v3-native -C Debug --output-on-failure
- [ ] API-side baseline or smoke verification is updated when API contracts are touched.

## Evidence

- [ ] Update artifacts/native/031-unified-coherent-world/umbrella-baseline.txt.
- [ ] Update artifacts/native/031-unified-coherent-world/full-engine-baseline.txt.
- [ ] Update artifacts/native/031-unified-coherent-world/full-mmo-demo.txt when MMO flow changes.
- [ ] Update API baseline artifacts when API contracts/tests change.

## Governance

- [ ] Update .specify/specs/031-unified-coherent-world/tasks.md with completed and pending items.
- [ ] Update .specify/specs/execution-dashboard.md status row for 031.
- [ ] Confirm no new docs or tasks instruct extending superseded slices 015-030.
