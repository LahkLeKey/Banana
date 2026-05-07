# Plan: CI Training Session Persistence

**Branch**: 039-ci-training-session-persistence | **Date**: 2026-04-26

## Objective

Persist deterministic CI training sessions into repository data folders so training can be replayed across CI, local, and container environments with consistent lineage.

## Scope

- Add normalized persisted session records for banana, not-banana, and ripeness lanes.
- Keep paths portable and workspace-relative.
- Preserve existing training CLI contracts.
- Add validation for persisted session schema and append-only history behavior.

## Constraints

- No breaking changes to existing trainer CLI arguments.
- No absolute machine-specific paths in persisted sessions.
- Existing validators must continue to pass.
- Data history must remain append-only.

## Deliverables

- research.md
- data-model.md
- contracts/training-session-contract.md
- quickstart.md
- tasks.md

## Structure

- scripts training surfaces: scripts/train-banana-model.py, scripts/train-not-banana-model.py, scripts/train-ripeness-model.py
- CI surfaces: .github/workflows/train-not-banana-model.yml and related model-training workflow entries
- persistence surfaces under data/
- spec artifacts under .specify/specs/039-ci-training-session-persistence/

## Validation lane

- python scripts/validate-ai-contracts.py
- bash scripts/validate-spec-tasks-parity.sh --all
- trainer commands for all 3 lanes using ci profile and single session mode

## Out of scope

- UI for human-operated training control
- Model architecture changes
- serving/runtime rollout policies
