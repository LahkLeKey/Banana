# Scripts Inventory (v1 snapshot)

Source: `scripts/`. 42 scripts total. Roles:
- **canonical**: documented runtime/build entry point referenced by VS Code tasks, docs, or CI.
- **helper**: utility called by canonical scripts; not a public entry point.
- **orchestration**: drives GH CLI / multi-step automation.
- **survival**: must be copied into `.specify/scripts/` before v1 wipe.
- **deprecated-candidate**: referenced infrequently; v2 should re-evaluate.

## Build / native

| Script | Role | Notes |
| --- | --- | --- |
| `build-all.sh` | canonical | Orchestrates native + dotnet builds. |
| `build-native.bat` | canonical | Windows native build entry. |
| `run-api.sh` | canonical | Local ASP.NET run with `BANANA_NATIVE_PATH` set. |

## Test / coverage

| Script | Role | Notes |
| --- | --- | --- |
| `run-tests-with-coverage.sh` | canonical | Dotnet test + Cobertura merge. |
| `run-native-c-tests-with-coverage.sh` | canonical | Native C tests + lcov. |
| `check-banana-api-coverage-denominator.sh` | helper | Stable-denominator gate (see spec 014). |
| `check-dotnet-coverage-threshold.sh` | helper | Threshold gate. |
| `sync-banana-api-coverage-denominator.py` | helper | Maintains `coverage-denominator.json`. |
| `export-cobertura-uncovered-lines.py` | helper | Reporting. |

## Compose / runtime channels

| Script | Role | Notes |
| --- | --- | --- |
| `compose-apps.sh` | canonical | Brings up `apps` profile. |
| `compose-apps-down.sh` | canonical | Tear-down counterpart. |
| `compose-runtime.sh` | canonical | Full runtime stack. |
| `compose-tests.sh` | canonical | Tests profile. |
| `compose-electron.sh` | canonical | `electron-example` smoke (NOT desktop UI). |
| `compose-electron-desktop-wsl2.sh` | canonical | Ubuntu-side Electron desktop entry. |
| `compose-mobile-emulators-wsl2.sh` | canonical | Ubuntu-side mobile entry. |
| `compose-local-workflows.sh` | canonical | Workflow-local profile. |
| `compose-e2e-bootstrap.sh` | canonical | E2E setup. |
| `compose-e2e-teardown.sh` | canonical | E2E teardown. |
| `launch-container-channels-with-wsl2-electron.sh` | canonical | Windows-shell Electron launcher. |
| `launch-container-channels-with-wsl2-mobile.sh` | canonical | Windows-shell mobile launcher. |

## AI surface guards

| Script | Role | Notes |
| --- | --- | --- |
| `validate-ai-contracts.py` | survival | Wiki allowlist + prompt-wiki linkage validator. CI guard. |
| `wiki-consume-into-specify.sh` | survival | Mirrors `.wiki/` -> `.specify/wiki/human-reference/`. |
| `workflow-sync-wiki.sh` | survival | Sync `.specify/wiki/` -> `.wiki/` (prune-aware, allowlist-enforced). |
| `smoke-test-spec-driven-agents.sh` | helper | Validates agent attribution. |

## Workflow orchestration (GH CLI)

| Script | Role | Notes |
| --- | --- | --- |
| `workflow-orchestrate-sdlc.sh` | orchestration | SDLC incremental PR engine. |
| `workflow-orchestrate-triaged-item-pr.sh` | orchestration | Triaged-item PR engine. |
| `workflow-triage-idea-cloud.sh` | orchestration | Idea-cloud triage. |
| `workflow-persist-registry-history-pr.sh` | orchestration | Model registry history PR. |
| `workflow-ensure-speckit.sh` | helper | Pre-flight speckit CLI check. |
| `workflow-local-orchestrate-sdlc.sh` | helper | Local mirror of SDLC workflow. |
| `workflow-local-orchestrate-triaged-item-pr.sh` | helper | Local mirror of triage workflow. |
| `workflow-local-train-not-banana.sh` | helper | Local mirror of training workflow. |

## Not-banana feedback loop

| Script | Role | Notes |
| --- | --- | --- |
| `apply-not-banana-feedback.py` | helper | Applies approved feedback to corpus. |
| `orchestrate-not-banana-feedback-loop.sh` | orchestration | End-to-end loop. |
| `train-not-banana-model.py` | canonical | Trainer; emits vocabulary header consumed by native. |
| `manage-not-banana-model-image.py` | helper | Registry image lifecycle. |
| `restore-not-banana-model-history.sh` | helper | Restore from registry snapshot. |

## Agent pulse / deterministic plan

| Script | Role | Notes |
| --- | --- | --- |
| `build-deterministic-agent-pulse-plan.py` | helper | Renders deterministic plan. |
| `generate-deterministic-agent-pulse-plan.sh` | helper | Wrapper around the above. |
| `record-agent-pulse-activity.py` | helper | Activity recorder. |

## v2 Notes

- Survival scripts MUST be copied into `.specify/scripts/` before deletion.
- The compose-* + launch-* scripts together form the canonical channel
  catalog enumerated in `.specify/specs/012-compose-runtime/contracts/channel-scripts.md`.
- The `workflow-orchestrate-*.sh` engines are the documented automation
  entry points (per `.github/copilot-instructions.md`); v2 must preserve
  them as orchestration entry points (renames OK).
