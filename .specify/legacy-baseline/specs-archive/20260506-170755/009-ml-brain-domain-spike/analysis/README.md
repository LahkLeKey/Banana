# SPIKE Analysis Index: ML Brain Domain

**Feature**: 009-ml-brain-domain-spike
**Date**: 2026-04-26
**Status**: In progress

## Purpose

This directory contains all SPIKE analysis artifacts for formalizing Banana's three ML brain domains and converting SPIKE outputs into implementation-ready follow-up packets.

## Domain Decisions

| Brain Domain | Model Family | Reasoning Style |
|---|---|---|
| Left Brain | Regression | Continuous estimation, calibration, magnitude |
| Right Brain | Binary | Categorical decisioning, thresholds, discrete classification |
| Full Brain | Transformer | Contextual synthesis, sequence interpretation |

## Artifact Index

### Phase 1 — Setup

| File | Purpose |
|---|---|
| [in-scope-files.md](in-scope-files.md) | Native/wrapper ML files in scope for this SPIKE |
| [validation-commands.md](validation-commands.md) | Lane command references for post-SPIKE validation |

### Phase 2 — Foundational Method

| File | Purpose |
|---|---|
| [terminology-rules.md](terminology-rules.md) | Canonical terminology definitions |
| [comparison-dimensions.md](comparison-dimensions.md) | Shared normalized dimensions for all family comparisons |
| [confidence-policy.md](confidence-policy.md) | Confidence and uncertainty policy |

### Phase 3 — Domain Definitions (US1)

| File | Purpose |
|---|---|
| [domain-definition-checklist.md](domain-definition-checklist.md) | US1 review checklist |
| [brain-domain-definitions.md](brain-domain-definitions.md) | Left/Right/Full canonical definitions |
| [model-family-mappings.md](model-family-mappings.md) | Primary family-to-domain mappings |
| [domain-boundaries.md](domain-boundaries.md) | Boundary rules and out-of-domain cases |
| [domain-risk-register.md](domain-risk-register.md) | Known risks per domain |

### Phase 4 — Tradeoff Matrix (US2)

| File | Purpose |
|---|---|
| [matrix-completeness-checklist.md](matrix-completeness-checklist.md) | US2 matrix coverage checklist |
| [model-family-tradeoff-matrix.md](model-family-tradeoff-matrix.md) | Normalized cross-family comparison matrix |
| [domain-fit-scorecards.md](domain-fit-scorecards.md) | Ranked fit scores per domain |
| [ranking-rationale.md](ranking-rationale.md) | Ranking rationale and tie-break policy |
| [uncertainty-impact-notes.md](uncertainty-impact-notes.md) | Uncertainty impacts on ranking |

### Phase 5 — Readiness Packets (US3)

| File | Purpose |
|---|---|
| [readiness-packet-checklist.md](readiness-packet-checklist.md) | US3 readiness quality checklist |
| [readiness-packet-left-brain.md](readiness-packet-left-brain.md) | Left Brain follow-up packet |
| [readiness-packet-right-brain.md](readiness-packet-right-brain.md) | Right Brain follow-up packet |
| [readiness-packet-full-brain.md](readiness-packet-full-brain.md) | Full Brain follow-up packet |
| [readiness-packets.md](readiness-packets.md) | Consolidated packet bundle |
| [validation-lane-plan.md](validation-lane-plan.md) | Lane expectations and contract impacts |

### Phase 6 — Closure

| File | Purpose |
|---|---|
| [success-criteria-evidence.md](success-criteria-evidence.md) | Cross-checked success criteria evidence |
