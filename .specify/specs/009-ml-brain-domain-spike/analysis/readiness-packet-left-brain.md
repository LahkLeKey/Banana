# Readiness Packet: Left Brain Domain

**Domain**: Left Brain
**Model Family**: Regression
**Date**: 2026-04-26
**Confidence**: High [H]

---

## Objective

Implement and validate the Left Brain Regression domain in Banana's native layer: a calibrated scalar scoring model that produces continuous ripeness and quality estimates, integrated with the wrapper ABI and exercised by native lane tests.

---

## Scope Boundary

### In Scope (Follow-Up Slice)

- Native regression model implementation or integration under `src/native/core/domain/ml/regression/`.
- Wrapper ABI exports for Left Brain scoring under `src/native/wrapper/domain/ml/regression/`.
- Native lane tests for regression output range, calibration spot-checks, and out-of-range clamping behavior.
- Feature vector input contract definition for the Left Brain domain.
- Documentation of the regression output contract (expected range, consumer usage pattern).

### Out of Scope (Follow-Up Slice)

- Classifier models (Right Brain domain).
- Transformer models (Full Brain domain).
- Changes to ASP.NET API pipeline unless the regression output contract changes the HTTP response shape.
- UI changes.

---

## Validation Lane Expectations

| Lane | Required | Key Assertions |
|---|---|---|
| Native (`ctest`) | Yes | Regression output in defined range (e.g., 0.0–1.0); behavior at boundary inputs (min, max, out-of-range); clamping contract |
| .NET Unit | Yes | Signature stability of wrapper interop for regression score; status code contract |
| .NET E2E Contracts | Recommended | HTTP response shape stability when regression score is surfaced through API |
| Coverage | Yes | Regression path must meet or contribute toward 80% coverage threshold |

---

## Risk Items from Risk Register

| Risk ID | Risk | Mitigation |
|---|---|---|
| LB-R01 | Ripeness scoring may not be calibrated for extreme inputs | Include boundary test vectors in native lane |
| LB-R02 | Out-of-range scores if distribution shifts | Add output clamping contract at wrapper ABI |
| LB-R03 | Feature engineering is brittle | Document feature contract explicitly in this slice |
| LB-R04 | Consumer misapply threshold on scalar output | Add consumer contract note in wrapper ABI header |
| U-002 | Runtime calibration unmeasured | Budget calibration validation test in native lane |

---

## First Deliverable

Define the regression input feature contract and output range contract before any model code — this is the domain boundary that all other Left Brain work depends on.

---

## Bounded Slice Summary

> Implement a regression-based ripeness scorer in the native C layer, export via wrapper ABI, test with native lane (output range + clamping), and document the consumer usage pattern. No cross-domain integration required in this slice.
