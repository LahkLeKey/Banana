# Feature Specification: Banana Contrastive Negative Hardening (254)

**Feature Branch**: `254-banana-contrastive-negative-hardening`  
**Created**: 2026-05-05  
**Status**: Draft

## Problem Statement

Generalization fails when near-neighbor fruits or banana-adjacent products appear (plantain, banana candy, banana-flavored products, mixed fruit contexts). We need deliberate contrastive training pressure.

## Goal

Build a contrastive negative lane that targets banana lookalikes and language traps to reduce false positives while preserving banana recall.

## In Scope

- Add hard-negative corpus pack and labels.
- Add confusion-driven replay set from reinforcement disagreements.
- Add precision floor gate for contrastive contexts.

## Out of Scope

- Image-modal contrastive learning.
- New serving infrastructure.

## Success Criteria

- SC-001: Hard-negative pack exists with category tags.
- SC-002: Evaluation reports precision/recall by hard-negative category.
- SC-003: False-positive rate in contrastive set decreases versus baseline.
- SC-004: Gate prevents promotion when contrastive precision floor fails.

## Assumptions

- Existing trainer can ingest additional labeled text samples.
- Baseline metrics are available from prior artifact snapshots.
