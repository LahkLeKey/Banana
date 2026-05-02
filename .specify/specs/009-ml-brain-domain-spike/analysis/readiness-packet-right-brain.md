# Readiness Packet: Right Brain Domain

**Domain**: Right Brain
**Model Family**: Binary
**Date**: 2026-04-26
**Confidence**: High [H]

---

## Objective

Implement and validate the Right Brain Binary domain in Banana's native layer: a calibrated binary classifier that produces discrete is-banana and ripe/unripe decisions, integrated with the wrapper ABI and exercised by native lane tests covering precision/recall operating point and negative-sample behavior.

---

## Scope Boundary

### In Scope (Follow-Up Slice)

- Native binary classification model implementation or integration under `src/native/core/domain/ml/binary/`.
- Wrapper ABI exports for Right Brain decisions under `src/native/wrapper/domain/ml/binary/`.
- Native lane tests for: class label output, class probability, negative sample coverage, threshold behavior.
- Calibration method selection and initial configuration (Platt scaling or isotonic regression).
- Operating point documentation (precision/recall target for the Banana is-banana use case).
- Feature vector input contract definition for the Right Brain domain.

### Out of Scope (Follow-Up Slice)

- Regression models (Left Brain domain).
- Transformer models (Full Brain domain).
- UI-level classification display (separate React slice).
- Changes to ASP.NET pipeline unless binary decision output changes HTTP response shape.

---

## Validation Lane Expectations

| Lane | Required | Key Assertions |
|---|---|---|
| Native (`ctest`) | Yes | Binary output is valid class label; class probability in [0,1]; negative-sample classification (non-banana produces is-banana=false); threshold behavior at operating point |
| .NET Unit | Yes | Signature stability of wrapper interop for binary decision; status code contract for binary result |
| .NET E2E Contracts | Recommended | HTTP response shape stability when binary decision is surfaced through API |
| Coverage | Yes | Binary classification path must meet or contribute toward 80% coverage threshold |

---

## Risk Items from Risk Register

| Risk ID | Risk | Mitigation |
|---|---|---|
| RB-R01 | Binary threshold may need per-deployment tuning | Expose threshold as configurable parameter; default to conservative operating point |
| RB-R02 | Precision/recall balance unclear | Define operating point explicitly as first deliverable; consult product requirements |
| RB-R03 | Multi-class conflated with binary | Apply Right Brain boundary rule: any fixed-class count is Right Brain |
| RB-R04 | False negatives on is-banana have downstream cost | Make negative-sample test case a required acceptance scenario, not optional |
| U-002 | Runtime calibration unmeasured | Budget Platt scaling calibration step and calibration test in native lane |

---

## First Deliverable

Define the binary classification operating point (precision/recall target) and input feature contract before implementing the model — this sets the acceptance scenario thresholds for the native lane tests.

---

## Bounded Slice Summary

> Implement a binary is-banana classifier in the native C layer, include calibration, export via wrapper ABI, test with native lane (class label + negative-sample coverage + operating point), and document the operating point configuration. No cross-domain integration required in this slice.
