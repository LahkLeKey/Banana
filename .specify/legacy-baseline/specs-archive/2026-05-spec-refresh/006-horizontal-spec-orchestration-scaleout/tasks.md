# Tasks: Horizontal Spec Orchestration Scale-Out

**Input**: Design documents from `.specify/specs/006-horizontal-spec-orchestration-scaleout/`
**Prerequisites**: plan.md, spec.md

## Phase 1: Shard Planning Contract

- [x] T001 [US1] Define shardability criteria (independence, ownership, conflict triggers) for active spec slices.
- [x] T002 [US1] Add deterministic shard-plan schema with explicit parallel vs sequential assignments.

## Phase 2: Confidence and Extension Safety

- [x] T003 [US2] Define per-shard confidence heartbeat minimum fields and storage paths.
- [x] T004 [US2] Add per-shard below-80% human checkpoint contract.
- [x] T005 [US2] Add extension-capability preflight contract for shard admission.

## Phase 3: Aggregate Reporting

- [x] T006 [US3] Define deterministic shard-merge report schema and stable ordering rules.
- [x] T007 [US3] Add blocker taxonomy (checkpoint-pending, extension-unhealthy, conflict-sequenced).

## Phase 4: Validation and Evidence

- [x] T008 [US1] Run boundary + traceability validators for this feature.
- [x] T009 [US3] Capture dry-run example with at least two parallel shards and one sequenced shard.
