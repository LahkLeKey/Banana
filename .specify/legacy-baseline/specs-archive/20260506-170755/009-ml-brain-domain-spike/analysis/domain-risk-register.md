# Domain Risk Register: ML Brain Domain SPIKE

**Date**: 2026-04-26
**Purpose**: Track known risks per brain domain for use in readiness packet planning.

## Left Brain (Regression) Risks

| Risk ID | Risk | Likelihood | Impact | Mitigation |
|---|---|---|---|---|
| LB-R01 | Ripeness scoring range may not be well-calibrated for edge cases (over-ripe, black banana) | Medium | Medium | Include calibration validation in follow-up Left Brain slice; test score range at extremes |
| LB-R02 | Regression model may produce out-of-range scores if input distribution shifts | Medium | High | Add output clamping contract at wrapper boundary; include distribution check in native lane |
| LB-R03 | Feature engineering for regression is manual and brittle | High | Medium | Document feature contract explicitly in follow-up slice; avoid implicit feature assumptions |
| LB-R04 | Left Brain output consumers may misapply threshold branching on scalar output | Low | Medium | Add consumer contract note in wrapper ABI; document expected consumer pattern in readiness packet |

## Right Brain (Binary) Risks

| Risk ID | Risk | Likelihood | Impact | Mitigation |
|---|---|---|---|---|
| RB-R01 | Binary classifier threshold may need per-deployment tuning | High | Medium | Expose threshold as configurable parameter; default to conservative operating point |
| RB-R02 | Precision/recall balance unclear for current banana use cases | Medium | High | Define operating point (e.g., prefer recall for safety-critical) explicitly in follow-up slice |
| RB-R03 | Multi-class extension (ripe/unripe/overripe) may be conflated with binary | Medium | Low | Document Right Brain boundary rule (any fixed-class count) clearly in contract |
| RB-R04 | False negatives on is-banana (non-banana classified as banana) may have downstream cost | High | High | Priority acceptance scenario in follow-up Right Brain slice; test with negative samples explicitly |

## Full Brain (Transformer) Risks

| Risk ID | Risk | Likelihood | Impact | Mitigation |
|---|---|---|---|---|
| FB-R01 | Transformer not yet implemented in Banana native layer | High | High | Readiness packet scopes implementation from scratch; treat as greenfield in follow-up slice |
| FB-R02 | Compute cost of transformer inference is significantly higher than Regression/Binary | High | High | Evaluate quantized inference and batch scheduling in follow-up Full Brain research sub-task |
| FB-R03 | Explainability of transformer outputs requires specialized tooling | High | Medium | Include attention-map logging as explicit follow-up lane requirement |
| FB-R04 | Transformer validation requires embedding-level test assertions; existing native lane patterns insufficient | High | High | Define new test pattern in readiness packet; do not reuse existing native assertion templates |
| FB-R05 | Contextual input format (embedding tokens vs raw features) must be standardized before implementation | Medium | High | Define input format contract as first deliverable of Full Brain follow-up slice |

## Cross-Domain Risks

| Risk ID | Risk | Likelihood | Impact | Mitigation |
|---|---|---|---|---|
| XD-R01 | Left Brain scores used as Right Brain inputs may propagate calibration errors into classification | Medium | High | Add integration test that feeds Left Brain output range into Right Brain and checks class stability |
| XD-R02 | Full Brain receiving Left+Right outputs as context may amplify upstream errors | Low | High | Gate Full Brain input context on Left+Right validation passing before synthesis |
| XD-R03 | Semantic drift between domain definitions across feature branches | Low | Medium | Enforce terminology-rules.md reference in all follow-up slice plan.md documents |
