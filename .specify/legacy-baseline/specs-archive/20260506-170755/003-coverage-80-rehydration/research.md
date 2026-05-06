# Research: Coverage 80 Rehydration for Entire Monorepo

**Date**: 2026-04-25
**Spec**: `.specify/specs/003-coverage-80-rehydration/spec.md`

## Decision 1: Enforce 80 percent by (domain, layer, lane) tuple

- **Decision**: Apply threshold enforcement at the tuple level (`domain`, `layer`, `lane`) instead of only aggregate rollup level.
- **Rationale**: Aggregate-only gating can hide weak domains behind stronger ones and fails the attribution goal.
- **Alternatives considered**:
  - Aggregate-only threshold: rejected because regressions can be masked.
  - Per-domain only threshold: rejected because layer-level drift still becomes opaque.

## Decision 2: Normalize all ecosystems into one lane result schema

- **Decision**: Convert native, .NET, and TypeScript coverage outputs into one canonical `CoverageLaneResult` schema before gating.
- **Rationale**: Coverage tools emit different formats; normalization is required for deterministic comparisons and summary publication.
- **Alternatives considered**:
  - Keep native/.NET/TS raw formats and compare ad hoc: rejected due parser drift and brittle triage.
  - Build separate gate logic per ecosystem: rejected due duplicated policy and inconsistency risk.

## Decision 3: Treat missing or unparseable coverage artifacts as contract failures

- **Decision**: Classify missing, empty, or invalid coverage artifacts as `coverage_contract_violation`.
- **Rationale**: A run that cannot prove coverage must not pass merge gates silently.
- **Alternatives considered**:
  - Soft warning on missing coverage: rejected because this allows silent quality erosion.
  - Retry-only behavior with no failure class: rejected because first-pass triage quality drops.

## Decision 4: Standardize denominator policy and publish snapshots

- **Decision**: Define one denominator policy for excluded categories (generated, vendored, third-party, build artifacts) and attach policy snapshots in evidence bundles.
- **Rationale**: Coverage percentages are meaningful only when denominator rules are explicit and stable across lanes.
- **Alternatives considered**:
  - Per-lane custom exclusions: rejected because percentages become incomparable.
  - Omit policy evidence from artifacts: rejected because auditability is lost.

## Decision 5: Represent non-applicable surfaces explicitly

- **Decision**: Record domain-layer tuples with no valid surface as `not-applicable` with rationale.
- **Rationale**: Omitted tuples are ambiguous and can be misinterpreted as accidental gaps.
- **Alternatives considered**:
  - Drop non-applicable tuples from output: rejected because aggregate transparency is reduced.
  - Force zero percent for non-applicable tuples: rejected because it produces false failures.

## Decision 6: Reuse 002 path-safety and preflight contracts

- **Decision**: Reuse lane evidence path and preflight classifications from 002 (`preflight_contract_violation` remains a first-class outcome).
- **Rationale**: Coverage stability depends on the same runtime dependency guarantees already established in 002.
- **Alternatives considered**:
  - New standalone path and preflight conventions: rejected due duplicated contract surface and drift risk.
  - Best-effort preflight only: rejected because runtime dependency failures become non-deterministic.

## Decision 7: Add explicit failure taxonomy

- **Decision**: Every failure is classified as one of `threshold_violation`, `coverage_contract_violation`, `preflight_contract_violation`, or `flake_contract_violation`.
- **Rationale**: Class taxonomy makes automation and one-pass triage deterministic.
- **Alternatives considered**:
  - Free-form reason strings only: rejected because category-level reporting becomes unreliable.
  - One generic failure class: rejected because remediation paths differ materially.

## Decision 8: Gate e2e coverage from declared flow manifests

- **Decision**: Compute e2e denominators from machine-readable flow manifests per runtime channel.
- **Rationale**: E2e coverage should reflect exercised declared user/runtime flows, not only low-level line counts.
- **Alternatives considered**:
  - Infer e2e denominator from test count alone: rejected because test count does not equal flow coverage.
  - Manual flow mapping in docs only: rejected because CI cannot enforce it deterministically.

## Decision 9: Enforce time-bounded exception governance

- **Decision**: Exceptions require owner, rationale, expiration date, remediation plan, and targeted tuple; expired or malformed exceptions do not suppress failures.
- **Rationale**: Exceptions are operationally necessary but must remain temporary and accountable.
- **Alternatives considered**:
  - Open-ended exception records: rejected because debt accumulates invisibly.
  - No exceptions at all: rejected because migration windows are sometimes required.

## Decision 10: Verify determinism with repeat-run windows

- **Decision**: Validate key outcomes over baseline 10-run windows where feasible.
- **Rationale**: Single-run success does not prove determinism in CI.
- **Alternatives considered**:
  - One-off validation only: rejected because flaky behavior can pass once and fail later.
  - Very large mandatory windows on every change: rejected due operational cost; use focused verification windows.
