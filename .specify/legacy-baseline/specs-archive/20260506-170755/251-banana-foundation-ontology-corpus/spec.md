# Feature Specification: Banana Foundation Ontology Corpus (251)

**Feature Branch**: `251-banana-foundation-ontology-corpus`  
**Created**: 2026-05-05  
**Status**: Draft

## Problem Statement

Current reinforcement suites capture verdict deltas, but they do not guarantee coverage of the foundational concept set a 5th-grade learner expects: fruit identity, plant growth context, ripeness cues, peel behavior, edibility/sweetness, and baseline nutrition facts such as potassium.

## Goal

Define and enforce a canonical banana ontology pack that turns "banana knowledge" into explicit, testable concept lanes rather than ad-hoc prompt text.

## In Scope

- Define a canonical concept taxonomy for banana fundamentals.
- Add concept-tagged training examples for each concept lane.
- Add metadata schema for concept tags and evidence source.
- Add corpus validation that fails when concept lanes are under-covered.

## Out of Scope

- Advanced botany beyond grade-school scope.
- New model architecture.
- UI redesign.

## Success Criteria

- SC-001: Canonical ontology JSON exists with at least 6 foundational lanes.
- SC-002: Corpus samples include concept tags tied to ontology IDs.
- SC-003: Validation reports per-lane counts and fails if any lane is below threshold.
- SC-004: Training artifacts include ontology coverage summary.

## Assumptions

- Existing corpus loaders can tolerate additional metadata fields.
- Coverage thresholds can start conservative and tighten over time.
