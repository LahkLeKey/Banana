# Feature Specification: Banana Concept Generalization Benchmark (252)

**Feature Branch**: `252-banana-concept-generalization-benchmark`  
**Created**: 2026-05-05  
**Status**: Draft

## Problem Statement

The current loop measures agreement/disagreement rates but not whether the model generalizes banana concepts across diverse contexts (biology, food, classroom definitions, nutrition, metaphors, and distractors).

## Goal

Establish a concept-generalization benchmark that measures robustness across varied contexts instead of only narrow phrasing.

## In Scope

- Build benchmark set grouped by concept lane and context family.
- Add scorecards for per-lane recall and cross-context consistency.
- Add CI regression gate on concept-generalization floor.

## Out of Scope

- External hosted benchmark service.
- Human annotation UI.

## Success Criteria

- SC-001: Benchmark set includes at least 8 context families.
- SC-002: Evaluation script outputs per-lane and per-context metrics.
- SC-003: CI fails when any lane falls below configured floor.
- SC-004: Artifact includes top confusion clusters.

## Assumptions

- Existing challenge-set scoring patterns can be adapted.
- Deterministic output format is required for CI diffs.
