# Plan: Human Left/Right Brain Training Workbench

**Branch**: 040-human-left-right-brain-training-workbench | **Date**: 2026-04-26

## Objective

Provide a human-operated workflow to configure, execute, review, and persist left-brain and right-brain training sessions using the persisted-session contracts from spec 039.

## Scope

- Add workbench experience for selecting lane, corpus, profile, session mode, and run limits.
- Surface live/final metrics and pass/fail threshold outcomes.
- Persist and promote selected runs with clear operator audit fields.
- Provide recovery guidance for failed runs.

## Constraints

- Reuse existing training scripts and promotion guardrails.
- No model architecture changes.
- Preserve existing API and workflow contracts unless explicitly extended.
- Training persistence behavior must remain compatible with spec 039.

## Deliverables

- research.md
- data-model.md
- contracts/training-workbench-contract.md
- quickstart.md
- tasks.md

## Structure

- React workbench surfaces under src/typescript/react/
- API orchestration surfaces under src/typescript/api/
- training script integrations under scripts/
- persisted-session references under data/

## Validation lane

- python scripts/validate-ai-contracts.py
- bash scripts/validate-spec-tasks-parity.sh --all
- app-level tests for touched React/API surfaces
- trainer flow verification for left/right lanes

## Out of scope

- full-brain transformer interactive training UX
- notebook-style experiment interfaces
- production rollout policy automation
