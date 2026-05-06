# Feature Specification: Autonomous Curriculum Expansion Loop (255)

**Feature Branch**: `255-autonomous-curriculum-expansion-loop`  
**Created**: 2026-05-05  
**Status**: Draft

## Problem Statement

We now auto-run suites and training, but we still lack a governed mechanism to expand curriculum breadth intentionally based on uncertainty, disagreement clusters, and concept coverage gaps.

## Goal

Create an autonomous curriculum expansion loop that proposes and stages new banana knowledge samples using measurable gap signals, with optional human cleanup later.

## In Scope

- Gap miner that identifies low-coverage concepts and unstable contexts.
- Candidate sample generator with provenance and confidence metadata.
- Staging lane for auto-proposed samples before promotion.
- Governance thresholds for automatic acceptance in autonomous mode.

## Out of Scope

- Full autonomous production promotion without rollback control.
- Rich review UI.

## Success Criteria

- SC-001: Gap miner emits ranked concept/context deficits.
- SC-002: Generator emits candidate samples with provenance and rationale.
- SC-003: Staging validator enforces schema/quality gates.
- SC-004: Autonomous mode can ingest safe candidates and retrain in one run.

## Assumptions

- Candidate quality can be approximated with deterministic heuristics first.
- Human review remains available for low-confidence candidates.
