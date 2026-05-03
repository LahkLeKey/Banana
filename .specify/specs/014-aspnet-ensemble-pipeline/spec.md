# Feature Specification: ASP.NET Ensemble Pipeline (Gated Cascade)

**Feature Branch**: `[014-aspnet-ensemble-pipeline]`
**Created**: 2026-04-26
**Status**: Draft
**SPIKE source**: [../013-ml-brain-composition-spike/analysis/followup-A-aspnet-ensemble.md](../013-ml-brain-composition-spike/analysis/followup-A-aspnet-ensemble.md)
## Problem Statement

Feature Specification: ASP.NET Ensemble Pipeline (Gated Cascade) aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope *(mandatory)*

- Implement gated-cascade composition across Right Brain (binary),
  Full Brain (transformer), and Left Brain (regression) as three new
  ordered ASP.NET pipeline steps.
- Introduce a single composite payload `EnsembleVerdictResult` returned
  by the new ensemble route on `BananaMlController`.
- Register the new steps in DI in cascade order: gating, escalation,
  calibration.

## Out of Scope *(mandatory)*

- Changes to `src/native/**` (no new exports; no ABI bump).
- Changes to existing pipeline steps or routes.
- Configurable cascade band (baked in at `[0.35, 0.65]`).
- React or Electron consumption (slice 015 / Electron later).
- New PostgreSQL queries.

## User Scenarios & Testing *(mandatory)*

### User Story 1 -- Cheap-path verdict (Priority: P1)

As an API consumer, when I submit a clearly-banana or clearly-not-banana
payload, I want the binary verdict returned without invoking the
transformer so the hot path stays cheap.

**Independent Test**: Pipeline emits a verdict and a "did_not_escalate"
flag; transformer step records zero invocations.

**Acceptance Scenarios**:
1. **Given** `{"text":"plastic engine oil junk waste motor oil"}`, **When**
   the ensemble route is called, **Then** label=`not_banana`,
   `did_escalate=false`, transformer call count = 0.
2. **Given** `{"text":"banana banana banana banana banana banana"}`,
   **When** the ensemble route is called, **Then** label=`banana`,
   `did_escalate=false`, transformer call count = 0.

---

### User Story 2 -- Ambiguous payload escalation (Priority: P1)

As an API consumer, when I submit an in-band payload (Right Brain
`banana_score in [0.35, 0.65]`), I want the transformer verdict to
override the binary verdict.

**Independent Test**: Pipeline records `did_escalate=true` and the
final label matches the transformer's verdict.

**Acceptance Scenarios**:
1. **Given** `{"text":"yellow plastic toy shaped like a banana"}`, **When**
   the ensemble route is called, **Then** label=`not_banana`,
   `did_escalate=true` (transformer overrides binary's permissive `banana`).
2. **Given** `{"text":"yellow fruit on the counter maybe"}`, **When**
   the ensemble route is called, **Then** label=`banana`,
   `did_escalate=true`.

---

### User Story 3 -- Calibration magnitude attached (Priority: P2)

As an API consumer, I always want the Left Brain regression score
attached to the response as a calibration magnitude, never as the source
of the label.

**Independent Test**: Response always contains `calibration_magnitude`
in `[0,1]`; the value never determines `label`.

**Acceptance Scenarios**:
1. **Given** any payload, **When** the ensemble route is called, **Then**
   `calibration_magnitude` is present and in `[0,1]`.

---

### User Story 4 -- Failure degradation (Priority: P3)

As an API consumer, when one brain fails, I want a degraded but explicit
response rather than a 500.

**Acceptance Scenarios**:
1. **Given** binary returns non-OK, **When** the ensemble route is called,
   **Then** the cascade falls through to the transformer.
2. **Given** both binary and transformer return non-OK, **When** the
   ensemble route is called, **Then** the response carries the regression
   score, label=`unknown`, and an explicit non-OK status code.

### Edge Cases

- `banana_score` exactly at `0.35` or `0.65`: inclusive of endpoints?
  Decision: band is `[0.35, 0.65]` inclusive on both ends.
- Empty `text`: routed to escalation (~0.5 binary score) -> transformer
  decides (~0.65 banana per 012 anchors) -> label=`banana`.

## Requirements *(mandatory)*

- **A-R01**: New steps MUST implement `IPipelineStep` and be registered
  in DI in cascade order (gating, escalation, calibration).
- **A-R02**: Cascade band `[0.35, 0.65]` MUST be a `const` in
  `EnsembleGatingStep`; no configurability in this slice.
- **A-R03**: Transformer step MUST NOT request diagnostics
  (`log_attention=0`, no embedding/attention buffers).
- **A-R04**: `EnsembleVerdictResult` JSON shape MUST be documented in
  XML doc comments and stable for 015 consumption:
  `{ label, score, did_escalate, calibration_magnitude, status }`.
- **A-R05**: Native lane MUST stay 7/7. ABI version unchanged at 2.2.
- **A-R06**: New unit + integration tests MUST cover all 6 anchor walks
  from
  [composition-strategy.md](../013-ml-brain-composition-spike/analysis/composition-strategy.md).

## Success Criteria

- All ctest 7/7 still passing.
- `dotnet test` green; coverage threshold preserved.
- 6 anchor walks asserted; failure-degradation path asserted.

## Validation lane

```
cmake --preset default && cmake --build --preset default
ctest --preset default --output-on-failure
dotnet test src/c-sharp/asp.net/asp.net.sln --collect:"XPlat Code Coverage"
bash scripts/check-dotnet-coverage-threshold.sh
```
