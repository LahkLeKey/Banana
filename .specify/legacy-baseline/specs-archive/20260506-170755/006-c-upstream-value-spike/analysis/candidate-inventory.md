# Candidate Inventory

Date: 2026-04-26
Spec: 006-c-upstream-value-spike

This inventory applies the inclusion/exclusion rules in `inventory-criteria.md` and focuses on bounded native-first slices that produce upstream value.

## CAND-001: Unified Operational JSON Contract Builder

- Title: Consolidate JSON payload assembly and schema guards for operational domains
- Problem statement: `banana_batch.c` and `banana_truck.c` each manually build JSON payloads with repeated `snprintf` patterns and ad hoc field naming.
- Native surface: `src/native/core/banana_batch.c`, `src/native/core/banana_truck.c`, `src/native/core/banana_ripeness.c`
- Expected upstream value: Lower malformed payload risk and fewer `invalid_native_payload` controller incidents in ASP.NET.
- Scope boundary: Add a shared native helper for safe payload construction and field consistency; no endpoint additions.
- Assumptions: Existing JSON response shapes remain backward compatible.

## CAND-002: Durable Operational State + Deterministic Reset Contract

- Title: Move operational state from process-static arrays to bounded durable state with test reset hooks
- Problem statement: Batch/harvest/truck rely on static process memory (`g_batches`, `g_harvest_batches`, `g_trucks`) with hard limits and runtime-coupled lifecycle.
- Native surface: `src/native/core/banana_batch.c`, `src/native/core/banana_truck.c`
- Expected upstream value: Higher operational reliability in long-running API processes and deterministic test/setup behavior.
- Scope boundary: Introduce bounded persistence abstraction plus deterministic reset API for test/runtime control.
- Assumptions: Follow-up may require wrapper + interop additions for reset/status administration.

## CAND-003: DAL Runtime Contract Hardening (Configured vs Linked vs Query Failure)

- Title: Strengthen Postgres DAL failure semantics and payload contract
- Problem statement: `banana_dal.c` currently returns success with a synthetic payload when libpq is not linked, which can mask environment/build issues.
- Native surface: `src/native/core/dal/banana_dal.c`, `src/native/core/banana_status.c`, `src/native/wrapper/banana_wrapper.h`
- Expected upstream value: Faster incident triage and fewer false-positive successful API responses under DAL misconfiguration.
- Scope boundary: Make DAL outcomes explicit and typed (configured, linked, query success/failure) without widening unrelated domains.
- Assumptions: ASP.NET status mapping and tests will be updated in lockstep.

## CAND-004: ML Output Contract Parity Across Regression/Binary/Transformer Paths

- Title: Normalize ML output metadata and confidence semantics
- Problem statement: ML paths expose different metadata slices, and ripeness currently wraps regression output with domain-specific labels only.
- Native surface: `src/native/core/domain/banana_ml_models.c`, `src/native/core/banana_ripeness.c`, `src/native/wrapper/domain/ml/*`
- Expected upstream value: More coherent upstream API/client handling and easier model comparison telemetry.
- Scope boundary: Align output metadata contract and confidence interpretation across model endpoints.
- Assumptions: No model algorithm rewrite in the first follow-up slice.

## CAND-005: ABI/Interop Surface Drift Guard Automation

- Title: Add explicit parity guard between `banana_wrapper.h`, native exports, and `INativeBananaClient`
- Problem statement: Interop method additions require synchronized updates across wrapper exports, P/Invoke declarations, interface, and test doubles.
- Native surface: `src/native/wrapper/banana_wrapper.h`, `src/native/wrapper/banana_wrapper.c`, `src/c-sharp/asp.net/NativeInterop/*`
- Expected upstream value: Reduced integration breakage and faster delivery for native feature expansion.
- Scope boundary: Add automated contract checks and structured failure diagnostics; do not redesign interop architecture.
- Assumptions: Existing single-interop-seam design remains unchanged.

## CAND-006: Operational Input Intent Parsing Hardening

- Title: Replace token heuristics with bounded field-aware intent parsing for batch/harvest/truck
- Problem statement: Current `count_term_ci`/`contains_ci` heuristics infer counts/locations from free-text tokens, risking unstable behavior with payload variation.
- Native surface: `src/native/core/banana_batch.c`, `src/native/core/banana_truck.c`
- Expected upstream value: More predictable operational outputs and lower confusion for API consumers.
- Scope boundary: Introduce bounded parser rules for key fields while preserving current endpoint set.
- Assumptions: JSON payloads remain small and deterministic parsing is preferred over ML for these operations.
