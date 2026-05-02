# Feature Specification: Drift Detection on Production Inference

**Feature Branch**: `078-training-drift-detection-on-inference`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 067 cross-domain follow-up planning)
**Wave**: fourth
**Domain**: api / training
**Depends on**: #061

## Problem Statement

There is no monitoring of input distribution or output entropy on production inference. A corpus / population shift would only surface as an accuracy regression in the next training cycle.

## In Scope *(mandatory)*

- Add a sampling middleware on both API surfaces that records input fingerprint + verdict distribution.
- Compute population stability index (PSI) + KL divergence vs. the training reference set on a schedule.
- Alert (via the audit log + observability surface) when drift exceeds a configurable threshold.
- Document the operator runbook for drift triage.

## Out of Scope *(mandatory)*

- Automated retraining on drift (decision stays human-in-the-loop in v1).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the cross-domain planning batch documented in [`.specify/specs/067-cross-domain-followup-planning/spec.md`](../067-cross-domain-followup-planning/spec.md).

## Status: Implemented
