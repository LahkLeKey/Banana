# Contract: One-Window Interactive Validation

## Purpose

Define the minimum contract for validating frontend and Electron behavior in one VS Code window before task closure.

## Actors

- Developer implementing frontend or Electron behavior changes.
- Reviewer validating closure evidence.

## Inputs

- Active feature tasks for a frontend/Electron slice.
- Canonical runtime/app profile entry points.
- VS Code integrated browser access to the running app surface.

## Required Sequence

1. Start runtime and app profiles from canonical repository entry points.
2. Open target frontend surface in VS Code integrated browser.
3. Validate rendered behavior relevant to changed scope.
4. Run in-window diagnostics using canonical drift diagnostic command/task when needed.
5. Record closure evidence and then execute canonical teardown.

## Required Assertions

- Rendered behavior assertion MUST exist for each frontend/Electron task closure.
- Env/container-level checks alone are insufficient when rendered behavior is part of the requirement.
- Drift diagnosis MUST identify failing layer or confirm clean state.

## Failure Contract

If rendered behavior does not match expected output:

- Task status remains open.
- Developer must execute documented remediation sequence.
- Closure evidence must include post-remediation rendered validation.

## Non-Goals

- This contract does not define backend-only closure requirements.
- This contract does not replace existing compose runtime contracts.
