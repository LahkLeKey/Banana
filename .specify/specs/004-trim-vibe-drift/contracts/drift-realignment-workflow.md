# Contract: Drift Realignment Workflow

## Purpose

Define the planning-time contract that keeps feature implementation aligned to approved specification scope.

## Inputs

Each planning cycle must provide:

- feature_spec_path (string, required)
- intake_issue_reference (string, required)
- user_story_list (array[string], required)
- requirement_list (array[string], required)
- proposed_task_list (array[string], required)
- owner_lane_candidates (array[string], required)

## Drift Detection Rules

- A proposed task is in-scope only if it maps to at least one approved requirement.
- A proposed task without requirement linkage must be marked as drift.
- Drift findings must be categorized as one of: missing-traceability, out-of-scope, dependency-mismatch, or ownership-mismatch.
- `.wiki` markdown files not listed in `.specify/wiki/human-reference-allowlist.txt` must be marked as out-of-scope drift until explicitly approved.

## Decision Output Schema

For each drift finding, produce:

- finding_id (string, required)
- decision_type (enum: retain, defer, split, reject, required)
- rationale (string, required)
- follow_up_destination (string, required for defer and split)
- decision_owner (string, required)
- decision_timestamp_utc (string, required)

## Traceability Output Schema

A planning-ready traceability map entry includes:

- user_story_id (string, required)
- requirement_id (string, required)
- planned_task_id (string, required)
- implementation_owner (string, required)
- validation_owner (string, required)
- status (enum: proposed, approved, deferred, required)

## Behavioral Guarantees

- Scope boundaries remain explicit for every planning cycle.
- Drift findings are never silently discarded.
- Deferred or split work always has a documented destination.
- Approved tasks remain traceable from intake issue through planning outputs.
- Canonical wiki mirror parity between `.wiki/` and `.specify/wiki/human-reference/` is validated as part of drift realignment governance.
