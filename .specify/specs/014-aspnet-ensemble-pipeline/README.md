# 014 ASP.NET Ensemble Pipeline -- Execution Tracker

**Status**: NOT STARTED. Ready to pick up.

## Source

Built from
[../013-ml-brain-composition-spike/analysis/followup-A-aspnet-ensemble.md](../013-ml-brain-composition-spike/analysis/followup-A-aspnet-ensemble.md).

## Anchor walks (must all pass in tests)

| Payload (text)                                        | Right band? | Final label  | did_escalate |
|-------------------------------------------------------|-------------|--------------|--------------|
| `ripe banana peel smoothie banana bunch banana bread` | no (>0.65)  | banana       | false        |
| `banana banana banana banana banana banana banana banana` | no (>0.65) | banana   | false        |
| `plastic engine oil junk waste motor oil`             | no (<0.35)  | not_banana   | false        |
| `yellow plastic toy shaped like a banana` (decoy)     | yes         | not_banana   | true         |
| `yellow fruit on the counter maybe`                   | yes         | banana       | true         |
| `""` (empty)                                          | yes         | banana       | true         |

## Phases (mapped to tasks.md)

1. Setup (T001-T003)
2. Foundational DTO + DI scaffolding (T004-T006)
3. US1 Cheap path (T007-T010)
4. US2 Escalation (T011-T014)
5. US3 Calibration (T015-T017)
6. US4 Degradation (T018-T020)
7. Integration (T021-T023)
8. Validation + close-out (T024-T026)

## Constraints

- No native changes; native lane stays 7/7 + ABI 2.2.
- No new managed test seams beyond the existing `INativeBananaClient`.
- Cascade band `[0.35, 0.65]` baked in (no config in this slice).
