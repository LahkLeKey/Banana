# Feature Specification: Native PGO + Performance Baseline

**Feature Branch**: `075-native-pgo-and-perf-baseline`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 067 cross-domain follow-up planning)
**Wave**: fourth
**Domain**: native
**Depends on**: #073

## Problem Statement

There is no performance baseline on the native pipeline; classifier latency drifts can land unnoticed.

## In Scope *(mandatory)*

- Add a benchmark harness (Google Benchmark or hyperfine) covering the hot paths.
- Land a PGO build pipeline (gcc/clang `-fprofile-generate` -> training corpus -> `-fprofile-use`).
- Publish a baseline artifact (latency percentiles + instruction count) on each main commit.
- CI lane fails if p95 regresses by >10% without an exception label.

## Out of Scope *(mandatory)*

- Hardware-specific tuning (-march=native is gated to local builds only).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the cross-domain planning batch documented in [`.specify/specs/067-cross-domain-followup-planning/spec.md`](../067-cross-domain-followup-planning/spec.md).

## Status: Implemented
