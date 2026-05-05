# Feature Specification: Banana Grade-School Explanation Quality (253)

**Feature Branch**: `253-banana-grade-school-explanation-quality`  
**Created**: 2026-05-05  
**Status**: Draft

## Problem Statement

The model can predict labels, but we do not yet score the quality of explanatory responses aligned to a 5th-grade understanding (clear, concise, factual, age-appropriate).

## Goal

Add an explanation-quality lane that evaluates whether generated banana explanations contain required core facts while remaining concise and readable.

## In Scope

- Add required-fact rubric (fruit, plant growth, yellow ripe cue, peel, edible/sweet, potassium nutrition).
- Add explanation generation evaluation script.
- Add readability + factual coverage checks.
- Add regression gate for explanation quality.

## Out of Scope

- Full pedagogy tutor UI.
- Multi-language localization.

## Success Criteria

- SC-001: Explanations are scored against required-fact rubric.
- SC-002: Readability target fits grade-school style envelope.
- SC-003: Missing required fact triggers non-zero exit in strict mode.
- SC-004: Summary includes coverage-by-fact and average length stats.

## Assumptions

- A compact explanation target range can be configured (for example 30-100 tokens equivalent).
- Readability heuristics can be approximate but deterministic.
