# Feature Specification: Orchestrate SDLC Workflow File Issue Fix

**Feature Branch**: `111-orchestrate-sdlc-workflow-file-issue-fix`
**Created**: 2026-05-02
**Status**: Follow-up stabilization scaffold
**Wave**: stabilization
**Domain**: workflow / automation
**Depends on**: #103, #106, #107

## Problem Statement

The `orchestrate-banana-sdlc` workflow is producing `workflow file issue` failures on push for `main`, indicating a workflow-definition or trigger compatibility defect that fails before any job execution. This creates persistent red checks and obscures actual automation health.

## In Scope *(mandatory)*

- Identify the exact workflow-definition defect causing push-time `workflow file issue` failure.
- Repair workflow syntax/semantics while preserving intended dispatch and schedule behavior.
- Add a workflow validation guard to catch similar definition defects pre-merge.
- Ensure autonomous SDLC orchestration paths remain functionally intact.

## Out of Scope *(mandatory)*

- Redesigning SDLC orchestration strategy.
- Disabling autonomous orchestration workflow entirely.

## Success Criteria

- No `workflow file issue` failure run is emitted for `.github/workflows/orchestrate-banana-sdlc.yml` on push.
- `workflow_dispatch` and scheduled orchestration behavior remain valid.
- Workflow validation catches definition regressions before merge.

## Notes for the planner

- Capture exact parser/validator signal used to identify the defect.
- Keep fix minimal and compatible with current ruleset + automation labels contract.
