# Banana

Pass the Banana from native C into API, desktop, web, and mobile channels.

This repo is a practical playground for one core idea: keep domain behavior in native C, then project it cleanly into multiple runtimes without rewriting business logic per app.

## Vibe Code Quickstart (5 Minutes)

1. Open this workspace in VS Code on Windows with Docker Desktop + WSL2 enabled.
2. In Run and Debug, pick `Demo Channel` from `.vscode/launch.json`.
3. Press F5 and wait for compose startup plus desktop/mobile launch hooks.
4. Open these endpoints:
   - API health: `http://localhost:8080/health`
   - React app: `http://localhost:5173`
   - React Native web: `http://localhost:19006`
5. Stop everything when done: `bash scripts/compose-apps-down.sh`.

## One-Click Runtime Channels

- Primary profile: `Demo Channel`
  - Starts API + app surfaces through Docker Compose.
  - Includes Electron desktop via WSL2/WSLg forwarding.
- Focused profiles:
  - `Build, API + Electron`
  - `Build, API + React-Native[Web]`
  - `Build, API + React-Native[Android]`
  - `Build, API + React`

## Mobile Runtime Behavior (WSL2/WSLg)

- Android startup path for the standard profile:
  - Tries Ubuntu Android SDK emulator first (native WSLg window).
  - Falls back to containerized `android-emulator` when local SDK launch is unavailable.
- noVNC fallback URL:
  - `http://localhost:6080/vnc.html?autoconnect=1&resize=remote&reconnect=1`
- iOS note:
  - iOS Simulator is macOS-only; Ubuntu channel uses iOS-style web preview fallback.

Optional helper launches:

- iOS-style preview + mobile helpers:
  - `bash -lc 'set -eo pipefail; scripts/launch-container-channels-with-wsl2-mobile.sh'`
- Ephemeral mobile session:
  - `bash -lc 'set -eo pipefail; BANANA_MOBILE_EPHEMERAL_SESSION=1 scripts/launch-container-channels-with-wsl2-mobile.sh'`

Useful environment overrides:

- `BANANA_SKIP_ANDROID_EMULATOR=1`
- `BANANA_ANDROID_OPEN_WINDOW=0`
- `BANANA_ANDROID_WSLG_WINDOW_URL=<url>`
- `BANANA_ANDROID_NOVNC_TIMEOUT_SEC=<seconds>`
- `BANANA_OPEN_RUNTIME_WINDOWS=0`
- `BANANA_ANDROID_RUNTIME_MODE=auto|local|container`
- `BANANA_IOS_PREVIEW_ENGINE=webkit`
- `BANANA_COMPOSE_APPS_SERVICES="api react-app"`

## Vibe Coding Guardrails

Keep these contracts steady while you iterate quickly:

- Architecture flow: `controller -> service -> pipeline -> native interop`.
- Native runtime path for managed integration: `BANANA_NATIVE_PATH`.
- PostgreSQL-backed native flows: `BANANA_PG_CONNECTION`.
- Frontend API base URL contract: `VITE_BANANA_API_BASE_URL`.

## Fast Developer Loop

1. Make a small change in one domain.
2. Reuse existing entry points instead of ad-hoc scripts:
   - Native build: `Build Native Library` task
   - API build: `Build Banana API` task
3. Run narrow tests first, then widen only when contract edges move.

## Incremental Not-Banana Training

Use one trainer for both local and CI sessions:

```bash
python scripts/train-not-banana-model.py \
  --corpus data/not-banana/corpus.json \
  --output artifacts/not-banana-model
```

Resource-aware session profiles:

- `--training-profile ci`: bounded sweep for constrained containers.
- `--training-profile local`: wider sweep for workstation runs.
- `--training-profile overnight`: broader sweep for long on-prem runs.
- `--training-profile auto`: chooses `ci` on GitHub Actions, otherwise scales by CPU count.

Session controls:

- `--session-mode incremental`: run multiple deterministic sessions and keep the best hold-out F1 result.
- `--session-mode single`: run one baseline session (fastest, deterministic).
- `--max-sessions <n>`: cap the number of evaluated sessions (`0` uses profile defaults).

Examples:

```bash
# Fast CI-like run
python scripts/train-not-banana-model.py --training-profile ci --session-mode incremental --max-sessions 4

# Longer overnight run
python scripts/train-not-banana-model.py --training-profile overnight --session-mode incremental --max-sessions 48
```

Artifacts are written to `artifacts/not-banana-model`:

- `vocabulary.json`: selected vocabulary + metrics + session history.
- `metrics.json`: selected hold-out metrics and per-session summary.
- `sessions.json`: detailed per-session hyperparameters and scores.
- `banana_signal_tokens.h`: generated native header.

### Model Image Registry (Safe Iteration + Rollback)

Package trainer outputs into immutable model images before promotion:

```bash
python scripts/manage-not-banana-model-image.py create \
  --model-dir artifacts/not-banana-model \
  --registry-dir artifacts/not-banana-model-registry \
  --channel candidate
```

Verify checksum integrity before promotion:

```bash
python scripts/manage-not-banana-model-image.py verify \
  --registry-dir artifacts/not-banana-model-registry \
  --channel candidate
```

Promote only after quality/safety checks:

```bash
python scripts/manage-not-banana-model-image.py promote \
  --registry-dir artifacts/not-banana-model-registry \
  --from-channel candidate \
  --to-channel stable \
  --min-f1 0.8 \
  --max-removed-tokens 4
```

Rollback is channel-pointer based and does not require retraining:

```bash
python scripts/manage-not-banana-model-image.py promote \
  --registry-dir artifacts/not-banana-model-registry \
  --image-id <previous-image-id> \
  --to-channel stable
```

Registry outputs are kept in `artifacts/not-banana-model-registry`:

- `images/<image-id>/`: immutable per-version files + `manifest.json`.
- `channels/candidate.json` and `channels/stable.json`: active pointers.
- `index.json`: lineage, metrics summary, and channel membership.

Manifest signing hardening:

- Set `BANANA_MODEL_SIGNING_KEY` (and optional `BANANA_MODEL_SIGNING_KEY_ID`) in CI secrets.
- Enforce signing at image creation:

```bash
python scripts/manage-not-banana-model-image.py create \
  --model-dir artifacts/not-banana-model \
  --registry-dir artifacts/not-banana-model-registry \
  --channel candidate \
  --require-signing-key \
  --signing-key-env BANANA_MODEL_SIGNING_KEY \
  --key-id-env BANANA_MODEL_SIGNING_KEY_ID
```

- Enforce signature verification before promotion:

```bash
python scripts/manage-not-banana-model-image.py verify \
  --registry-dir artifacts/not-banana-model-registry \
  --channel candidate \
  --require-signature \
  --signing-key-env BANANA_MODEL_SIGNING_KEY
```

Long-term snapshot persistence:

- Create snapshot archive + sha manifest:

```bash
python scripts/manage-not-banana-model-image.py snapshot \
  --registry-dir artifacts/not-banana-model-registry \
  --snapshot-dir artifacts/not-banana-model-registry-snapshots
```

- Optional object-store upload using pre-signed URLs:

```bash
python scripts/manage-not-banana-model-image.py snapshot \
  --registry-dir artifacts/not-banana-model-registry \
  --snapshot-dir artifacts/not-banana-model-registry-snapshots \
  --upload-url "<presigned archive PUT url>" \
  --upload-sha-url "<presigned sha PUT url>"
```

- In CI, snapshots can also be published as GitHub release assets via `publish_registry_release_asset=true`.

Multi-release CI builds:

- The training workflow now builds multiple release profiles in one run (stable candidate, canary wide, strict safety) by default.
- Override profiles using `release_matrix_json` (workflow dispatch input).

Example matrix payload:

```json
[
  {
    "release_id": "stable-candidate",
    "training_profile": "ci",
    "session_mode": "incremental",
    "max_sessions": "6",
    "vocab_size": "24",
    "min_signal_score": "0.70",
    "min_f1": "0.75",
    "allow_stable_promotion": "true"
  },
  {
    "release_id": "canary-wide",
    "training_profile": "ci",
    "session_mode": "incremental",
    "max_sessions": "10",
    "vocab_size": "40",
    "min_signal_score": "0.62",
    "min_f1": "0.70",
    "allow_stable_promotion": "false"
  }
]
```

Repository-backed long-term history:

- Set workflow input `persist_registry_history=true` to stage release snapshots into an automated pull request.
- Configure:
  - `registry_history_branch` (default: `model-release-history`, used as branch prefix)
  - `registry_history_path` (default: `data/not-banana/model-release-history`)
  - `registry_history_pr_base_branch` (default: `main`)
  - `registry_history_open_draft_pr` (default: `true`)
  - `registry_history_pr_labels` (default includes `requires-human-approval`)
  - `registry_history_pr_reviewers` (optional comma-separated usernames)
- CI writes timestamped snapshot bundles, updates `latest.json`, then opens a PR for human review.

Automation pull request orchestration for triaged code changes:

- Run workflow `Orchestrate Triaged Item Pull Request` with:
  - `triage_id`
  - `change_command`
  - optional branch/label/reviewer overrides
- The workflow creates a branch, commits generated changes, opens a PR, and labels it with `requires-human-approval` by default.

Human-approval merge gate:

- Workflow `Require Human Approval (Automation PRs)` blocks automation PRs until at least one non-bot approval is present.
- Add this check as required in your protected-branch ruleset so merge remains blocked until a human review approves.
- Keep branch settings aligned with `Require a pull request before merging` and reviewer requirements.
- Solo maintainer option: add label `solo-maintainer-bypass` to an owner-authored automation PR to bypass the gate when no second human reviewer exists.
- Safety rule: `solo-maintainer-bypass` fails the check if applied to a PR not opened by the repository owner.

Local workflow containers (Docker Compose):

- Use profile `workflow-local` to run one workflow validation path per container.
- Train + persistence PR path (dry-run orchestration by default):

```bash
bash scripts/compose-local-workflows.sh workflow-train-not-banana-local
```

- Triaged item PR path with safe no-op change command:

```bash
BANANA_TRIAGE_CHANGE_COMMAND=':' \
bash scripts/compose-local-workflows.sh workflow-orchestrate-triaged-local
```

- To run against GitHub for real PR creation, provide `GH_TOKEN` and set `BANANA_LOCAL_DRY_RUN=false`.

## Learn More

- Wiki hub: [Banana Wiki](https://github.com/LahkLeKey/Banana/wiki)
- Onboarding and architecture: `docs/developer-onboarding.md`
- Build/test command index: `.wiki/Build-Run-Test-Commands.md`

> Banana is a prototype for Poly: a polymorphic, multi-platform runtime approach where core behavior starts in C and can be projected into API, desktop, frontend, and mobile delivery channels.
