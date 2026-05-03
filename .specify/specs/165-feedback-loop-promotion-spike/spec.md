# Feature Specification: Feedback Loop Promotion Spike (165)

**Feature Branch**: `feature/165-feedback-loop-promotion-spike`
**Created**: 2026-05-02
**Status**: Ready for implementation
**Input**: Request to promote a tighter memory-first feedback loop while draining specs and evolving BananaAI + suite UX.

## Problem Statement

Spec-drain execution currently relies on periodic manual checkpoints rather than a first-class promotion loop that turns observed runtime/spec outcomes into prioritized follow-up slices. A formal feedback-loop spike is needed so insights from routing, deployment, and model UX can continuously feed backlog and automation.

## In Scope

- Define feedback-loop lifecycle: observe -> classify -> promote -> execute -> verify.
- Define artifact contract for promoted items (source signal, confidence, owner, next action).
- Define routing from promoted items into new Spec Kit slices.

## Out of Scope

- Full automation engine implementation for promotion.
- Cross-repo orchestration beyond Banana monorepo boundaries.

## Success Criteria

- A feedback-loop promotion contract is documented and compatible with current spec-drain artifacts.
- Promotion criteria include value, risk, and dependency unlock scoring.
- Follow-up implementation points are listed for scripts/workflows/wiki surfaces.

## Assumptions

- Existing drain artifacts under `artifacts/sdlc-orchestration/` remain the source of truth.
- Promotion outputs should stay machine-readable and repository-local.
