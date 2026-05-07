# PR Summary: 044 One-Window Interactive Baseline

## Why

Codify the VS Code integrated browser flow as the default frontend/Electron validation path so developers can launch, diagnose, and verify rendered behavior in one workspace window.

## What Changed

- Added governance baseline for one-window interactive validation in `.specify/memory/constitution.md` (version bump to 1.1.0).
- Added Copilot operational rule to default frontend/Electron UX validation to the VS Code integrated browser in `.github/copilot-instructions.md`.
- Added runbook guidance for one-window debugging loop in `.specify/wiki/human-reference/Build-Run-Test-Commands.md`.
- Created full Spec Kit feature artifacts for 044:
  - `spec.md`, `plan.md`, `research.md`, `data-model.md`, `contracts/one-window-validation-contract.md`, `quickstart.md`, `tasks.md`.
- Updated closure evidence requirements:
  - Quickstart now captures browser surface, rendered assertion outcome, and remediation result.
  - Spec now includes FR-007 requiring closure evidence when drift signals occur.
- Completed all 044 tasks and recorded validation evidence in `tasks.md`.

## Validation

Executed one command sequence for runtime + apps + drift diagnostics + teardown:

```bash
bash scripts/compose-run-profile.sh --profile runtime --action up
bash scripts/compose-profile-ready.sh --profile runtime
bash scripts/compose-run-profile.sh --profile apps --action up
bash scripts/compose-profile-ready.sh --profile apps
bash scripts/validate-frontend-config-drift.sh --require-running
bash scripts/compose-run-profile.sh --profile apps --action down
bash scripts/compose-run-profile.sh --profile runtime --action down
```

Observed results:

- Static contract checks: pass
- Interpolated compose checks: pass
- Running service env check: pass
- Output includes: `frontend config drift validation passed`
- Exit code: `0`

## Scope Notes

- Documentation/governance/process slice only.
- No backend API behavior changes.
- No native domain logic changes.

## Suggested PR Title

`docs(spec-044): baseline one-window interactive frontend validation in VS Code`

## Suggested Commit Message

`docs(spec-044): codify one-window interactive validation baseline and closure evidence`
