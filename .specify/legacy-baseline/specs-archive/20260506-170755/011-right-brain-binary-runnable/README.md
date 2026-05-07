# 011 Right Brain Binary Runnable C Code — Execution Tracker

Active feature pointer: `.specify/feature.json` -> `.specify/specs/011-right-brain-binary-runnable`.

## Goal

Lock the Right Brain (binary classifier) operating point, calibrate the
raw score, expose a per-deployment threshold knob, and field-lock the JSON
contract that downstream consumers (.NET interop, React/Electron clients)
deserialize.

## Phases

- Phase 1 (T001-T003) — Setup scaffolding (this tracker + analysis files).
- Phase 2 (T004-T006) — Foundational headers: operating point, JSON shape,
  affine calibration constants.
- Phase 3 (T007-T010) — US1 MVP: affine calibration applied + positive
  calibration anchor enforced.
- Phase 4 (T011-T014) — US2: negative-sample coverage (oil, empty,
  banana-adjacent).
- Phase 5 (T015-T018) — US3: configurable threshold via new
  `_with_threshold` core + wrapper + public ABI export, monotone sweep test.
- Phase 6 (T019-T021) — US4: locked JSON contract field-presence asserts
  + null-pointer guards.
- Phase 7 (T022-T025) — Native lane validation, evidence capture, closure.

## Calibration anchors (empirically derived)

| Variant                                      | Payload (text)                                                      | banana_score | label       |
|----------------------------------------------|---------------------------------------------------------------------|--------------|-------------|
| `banana_classify_banana_binary`              | `ripe banana peel smoothie banana bunch banana bread`               | ~0.944       | banana      |
| `banana_classify_banana_binary`              | `""` (empty)                                                        | ~0.583       | banana      |
| `banana_classify_banana_binary`              | `plastic engine oil junk waste motor oil`                           | ~0.093       | not_banana  |
| `banana_classify_not_banana_junk` (strict)   | `""` (empty)                                                        | ~0.431       | not_banana  |
| `banana_classify_not_banana_junk` (strict)   | `yellow plastic toy shaped like a banana`                           | ~0.381       | not_banana  |

The `_junk` variant uses a not-banana bias (0.18) and is the recommended
strict gate for false-positive-sensitive consumers (RB-R04).
