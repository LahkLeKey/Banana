# Feature Specification: Cross-Model Joint Training Pipeline

**Feature Branch**: `076-training-cross-model-joint-pipeline`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 067 cross-domain follow-up planning)
**Wave**: fifth
**Domain**: training
**Depends on**: #050, #079

## Problem Statement

Banana, not-banana, and ripeness train independently. Shared signals (image features, surprise weights, replay items) are duplicated and inconsistent. The neuro layer (feature 050) explicitly deferred this.

## In Scope *(mandatory)*

- Introduce a shared feature-extraction stage that all three trainers can consume.
- Add a joint-training profile that updates all three heads against a unified replay buffer.
- Preserve the per-model deterministic outputs (single-model trainers stay bit-for-bit identical).
- Document the conditions under which joint training is safe vs. risky.

## Out of Scope *(mandatory)*

- Replacing the existing per-model trainers (joint mode is additive).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the cross-domain planning batch documented in [`.specify/specs/067-cross-domain-followup-planning/spec.md`](../067-cross-domain-followup-planning/spec.md).

## Status: Implemented
