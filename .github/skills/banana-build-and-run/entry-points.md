# Build And Run Entry Points

## Native

- Workspace task: `Build Native Library`
- Script: `scripts/build-native.bat` on Windows
- Coverage build path: `scripts/run-native-c-tests-with-coverage.sh`
- Primary helpers: `native-core-agent`, `native-dal-agent`, `native-wrapper-agent`

## ASP.NET

- Workspace task: `Build Banana API`
- Full build script: `scripts/build-all.sh`
- Local run helper: `scripts/run-api.sh` when available in the current environment
- Primary helpers: `api-pipeline-agent`, `api-interop-agent`

## Frontend

- React dev: `bun run dev` in `src/typescript/react`
- React build: `bun run build` in `src/typescript/react`
- Electron smoke: `npm run smoke` in `src/typescript/electron`
- Primary helpers: `react-ui-agent`, `electron-agent`

## Compose And Runtime

- Full apps stack: `bash scripts/compose-apps.sh`
- Detached full apps stack: `DETACH=1 bash scripts/compose-apps.sh`
- Stop apps stack: `bash scripts/compose-apps-down.sh`
- Test profile: `bash scripts/compose-tests.sh`
- Runtime profile: `bash scripts/compose-runtime.sh`
- Local workflow containers: `bash scripts/compose-local-workflows.sh <service>`
- Local training PR-path service: `workflow-train-not-banana-local`
- Local triaged PR-path service: `workflow-orchestrate-triaged-local`
- Electron desktop channel: `bash -lc 'set -eo pipefail; scripts/launch-container-channels-with-wsl2-electron.sh'`
- Mobile emulator channel: `bash -lc 'set -eo pipefail; scripts/launch-container-channels-with-wsl2-mobile.sh'`
- Primary helpers: `compose-runtime-agent`, `mobile-runtime-agent`, `workflow-agent`

## Native ML Domain Contract (2026-04)

- Runtime and CI surfaces should preserve native ML build/test orchestration without owning model internals.
- Ensure existing tasks, scripts, and workflows still compile and exercise sources under `src/native/core/domain/ml/*` and `src/native/wrapper/domain/ml/*`.
- Keep native library loading and path assumptions explicit in runtime environments.
- Escalate model-logic changes to native helpers; keep runtime scope focused on orchestration, diagnostics, and delivery parity.

## Not-Banana Training Contract (2026-04)

- Keep training data, script, and workflow wiring coherent across `data/not-banana/corpus.json`, `scripts/train-not-banana-model.py`, and `.github/workflows/train-not-banana-model.yml`.
- Local and CI should use the same trainer entry point with explicit profile/session flags.
- CI-like incremental command: `python scripts/train-not-banana-model.py --training-profile ci --session-mode incremental --max-sessions 4`
- Overnight incremental command: `python scripts/train-not-banana-model.py --training-profile overnight --session-mode incremental --max-sessions 48`
- Single-session deterministic command: `python scripts/train-not-banana-model.py --session-mode single --max-sessions 1`
- Package immutable image command: `python scripts/manage-not-banana-model-image.py create --model-dir artifacts/not-banana-model --registry-dir artifacts/not-banana-model-registry --channel candidate`
- Verify image command: `python scripts/manage-not-banana-model-image.py verify --registry-dir artifacts/not-banana-model-registry --channel candidate`
- Signed create command: `python scripts/manage-not-banana-model-image.py create --model-dir artifacts/not-banana-model --registry-dir artifacts/not-banana-model-registry --channel candidate --require-signing-key --signing-key-env BANANA_MODEL_SIGNING_KEY --key-id-env BANANA_MODEL_SIGNING_KEY_ID`
- Signed verify command: `python scripts/manage-not-banana-model-image.py verify --registry-dir artifacts/not-banana-model-registry --channel candidate --require-signature --signing-key-env BANANA_MODEL_SIGNING_KEY`
- Promote to stable command: `python scripts/manage-not-banana-model-image.py promote --registry-dir artifacts/not-banana-model-registry --from-channel candidate --to-channel stable --min-f1 0.8 --max-removed-tokens 4`
- Rollback command: `python scripts/manage-not-banana-model-image.py promote --registry-dir artifacts/not-banana-model-registry --image-id <previous-image-id> --to-channel stable`
- Snapshot command: `python scripts/manage-not-banana-model-image.py snapshot --registry-dir artifacts/not-banana-model-registry --snapshot-dir artifacts/not-banana-model-registry-snapshots`
- Snapshot upload command: `python scripts/manage-not-banana-model-image.py snapshot --registry-dir artifacts/not-banana-model-registry --snapshot-dir artifacts/not-banana-model-registry-snapshots --upload-url <archive-put-url> --upload-sha-url <sha-put-url>`
- Restore latest source-controlled snapshot command: `python scripts/manage-not-banana-model-image.py restore-history --history-path data/not-banana/model-release-history --snapshot latest --output-root artifacts`
- Restore selected releases command: `python scripts/manage-not-banana-model-image.py restore-history --history-path data/not-banana/model-release-history --snapshot <timestamp> --release-id stable-candidate --release-id canary-wide --output-root artifacts`
- Restore wrapper script: `bash scripts/restore-not-banana-model-history.sh` (supports `BANANA_REGISTRY_RESTORE_*` env vars)
- Feedback apply command: `python scripts/apply-not-banana-feedback.py --feedback data/not-banana/feedback/inbox.json --corpus data/not-banana/corpus.json --status-filter approved`
- Feedback PR orchestration script: `bash scripts/orchestrate-not-banana-feedback-loop.sh`
- Feedback PR orchestration workflow: `.github/workflows/orchestrate-not-banana-feedback-loop.yml`
- CI multi-release build input: `release_matrix_json` (workflow dispatch) to build multiple model releases in one run.
- CI repository persistence input: `persist_registry_history=true` to create a registry-history pull request containing snapshot bundles at `registry_history_path`.
- CI repository persistence PR controls: `registry_history_pr_base_branch`, `registry_history_open_draft_pr`, `registry_history_pr_labels`, and `registry_history_pr_reviewers`.
- Push-based corpus persistence: when `data/not-banana/corpus.json` changes, `Train Not-Banana Model` now persists registry history automatically in the same run.
- Triaged-code PR orchestration workflow: `.github/workflows/orchestrate-triaged-item-pr.yml` via workflow dispatch with `triage_id` + `change_command`.
- Human-approval gate workflow: `.github/workflows/require-human-approval.yml` (mark check required in branch protection/rulesets).
- Preserve CI/container prerequisites needed to execute training and drift checks reliably.
- Treat training drift failures as actionable model/data contract signals, not infrastructure noise.
- Document any runtime or automation changes that affect training invocation, artifacts, or reproducibility.

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

## Cross-Domain Teaming Protocol

- Follow [domain-teaming-playbook.md](../../agents/domain-teaming-playbook.md) for ownership boundaries, handoff packet expectations, and escalation rules.
- Identify the primary owner, supporting helpers, and validation owner before implementation.
- When handing work to another agent, include objective, ownership reason, touched files, contract impacts, validation state, and open risks.
- Preserve context continuity by carrying forward confirmed assumptions and prior validation signals.
- Escalate to `banana-sdlc` for multi-domain implementation orchestration and `integration-agent` for multi-domain validation orchestration.

## Runtime Contract Lessons (2026-04)

- Treat `Banana Channels (Container Driven)` as the canonical one-click local runtime entry point: launch from a Windows shell with `scripts/launch-container-channels-with-wsl2-electron.sh` (or the VS Code profile) so Docker Desktop + Ubuntu-24.04/Ubuntu WSL2 runtime contracts stay enforced.
- Distinguish channels clearly: compose `electron-example` is a smoke container (`npm run smoke`) and is not the desktop UI runtime channel.
- For React container builds that consume `@banana/ui` via file dependency, keep `.dockerignore` excluding `**/node_modules` so host Windows symlinks do not overwrite Linux container installs.
- In `docker/react.Dockerfile`, copy shared UI package files before install and run `bun install --cwd /workspace/src/typescript/shared/ui` before app-level install so shared deps resolve at runtime.
- Keep Electron container builds reproducible with lockfile install (`npm ci --omit=dev`) and compatibility flags required by current native modules (`CXXFLAGS=-fpermissive`).
- Base-image scanner highs on Debian slim may remain; mitigate drift by pinning immutable image digests and tracking residual base CVEs as platform risk when no direct runtime exploit path is introduced.


## Ubuntu WSL2 Reproducible Contract (2026-04)

- Enforce the standard Windows + Docker Desktop + Ubuntu (Microsoft Store) workflow described in [ubuntu-wsl2-runtime-contract.md](../../ubuntu-wsl2-runtime-contract.md).
- Keep `scripts/launch-container-channels-with-wsl2-electron.sh` as the Windows-shell entry point and `scripts/compose-electron-desktop-wsl2.sh` as the Ubuntu entry point.
- Preserve Ubuntu-first distro selection order: `BANANA_WSL_DISTRO` override, then `Ubuntu-24.04`, then `Ubuntu`.
- Preserve strict direct container-to-WSLg rendering behavior and fail fast with actionable setup guidance when display or Docker integration prerequisites are missing.
- Treat missing Ubuntu Docker server/socket as an environment contract failure first; surface clear remediation and keep exit code `42` for integration preflight failures.
