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
- `--min-token-length <n>`: reject short low-information tokens from the learned vocabulary (default `3`).
- `--allow-numeric-tokens`: include numeric-only tokens in the learned vocabulary (disabled by default).

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
- `banana_signal_tokens.h`: generated native header. Copy to
  `src/native/core/domain/generated/banana_signal_tokens.h` to make native
  not-banana inference consume the trained vocabulary.

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
  - `registry_history_pr_reviewers` (optional comma-separated usernames or `["user1","user2"]` array syntax)
- CI writes timestamped snapshot bundles, updates `latest.json`, then opens a PR for human review.
- Reviewer requests are applied one username at a time; invalid or unavailable usernames are skipped with workflow warnings.

Restore persisted history after a fresh clone:

- Restore the latest snapshot listed in `data/not-banana/model-release-history/latest.json`:

```bash
bash scripts/restore-not-banana-model-history.sh
```

- Restore a specific timestamped snapshot:

```bash
BANANA_REGISTRY_RESTORE_SNAPSHOT=20260423T010945Z \
bash scripts/restore-not-banana-model-history.sh
```

- Restore only selected release ids from that snapshot:

```bash
BANANA_REGISTRY_RESTORE_RELEASE_IDS=stable-candidate,canary-wide \
bash scripts/restore-not-banana-model-history.sh
```

- Restored artifacts are written under `artifacts/not-banana-model`, `artifacts/not-banana-model-registry`, and `artifacts/not-banana-model-registry-snapshots`.
- By default, restore overwrites existing target release directories. Set `BANANA_REGISTRY_RESTORE_OVERWRITE=false` to fail instead.
- Restore writes a machine-readable report at `artifacts/not-banana-model-restore-report.json`.

Automation pull request orchestration for triaged code changes:

- Run workflow `Orchestrate Triaged Item Pull Request` with:
  - `triage_id`
  - `change_command`
  - optional branch/label/reviewer overrides
- The workflow creates a branch, commits generated changes, opens a PR, and labels it with `requires-human-approval`, `copilot-auto-approve`, and `copilot-bypass-vibe-coded` by default.
- The workflow now runs `scripts/workflow-sync-wiki.sh` in the same run by default (`sync_wiki=true`).
- Wiki remote defaults to `https://github.com/LahkLeKey/Banana.wiki.git` and enforces this canonical target by default (`BANANA_ENFORCE_CANONICAL_WIKI_REMOTE=true`).

Cloud idea triage from prompt or issue labels:

- Use prompt `/triage "<idea>"` to create a triage issue and kick off cloud orchestration.
- Workflow `Orchestrate Triage Idea Cloud` can also run via `workflow_dispatch` when you provide either `idea` or `issue_number`.
- Applying label `triage-idea`, `copilot-suggestion`, or `human-triage` to an issue triggers the same cloud workflow path.
- The workflow records intake artifacts under `docs/triage/intake/issue-<number>.md` and opens a triaged automation PR through the existing triaged-item PR engine.
- Default AI issue labels (workflow-created): `triage-idea`, `copilot-suggestion`, `ai-generated`, `automation`, `copilot-bypass-vibe-coded`.
- Human issue templates under `.github/ISSUE_TEMPLATE/human-*.yml` embed hidden source/agent routing markers so CI can classify `source-human-issue` and propagate `agent:*` labels to generated PRs.
- Default generated PR labels: `automation`, `triaged-item`, `copilot-auto-approve`, `copilot-autonomous-cycle`, `copilot-bypass-vibe-coded`.
- Default generated PR mode is ready-for-review (`draft_pr=false`) for autonomous continuation.
- Cloud triage now clears backlog by default before creating new artifacts (`clear_backlog=true`).
- Backlog cleanup targets open AI issues with labels `copilot-suggestion` or `ai-generated`, and open PRs with labels `automation` + `triaged-item` (or triage branch prefixes).
- Backlog cleanup scope can be tuned with workflow inputs `backlog_issue_labels`, `backlog_pr_labels`, and `backlog_pr_branch_prefixes`.
- Cloud triage now dispatches `copilot-review-triage.yml` and `require-human-approval.yml` via `workflow_dispatch` on generated PR heads so required checks report for bot-authored PRs.
- Required-check dispatch can be tuned with `dispatch_required_checks` and `check_workflows` workflow inputs (or `BANANA_TRIAGE_DISPATCH_REQUIRED_CHECKS` / `BANANA_TRIAGE_CHECK_WORKFLOWS`).
- The workflow supports custom issue labels and PR labels through workflow inputs.
- Wiki sync is enabled by default in the cloud triage workflow and uses canonical wiki remote enforcement.

Feedback-loop orchestration for classifier improvement:

- Queue reviewed classifier observations in `data/not-banana/feedback/inbox.json` with `status=approved`.
- Human oversight is enforced at ingestion time: approved entries must include
  `reviewed_by` and `reviewed_at_utc` metadata, or automation rejects them.
- Run workflow `Orchestrate Not-Banana Feedback Loop` to:
  - ingest approved feedback into `data/not-banana/corpus.json`
  - mark processed feedback entries as `applied` (configurable)
  - open an automation PR labeled `feedback-loop`
- Workflow inputs `require_human_review` (default `true`) and
  `min_human_reviewers` (default `1`) control this gate.
- The workflow also runs `scripts/workflow-sync-wiki.sh` by default (`sync_wiki=true`) and supports `wiki_remote_url` when an explicit wiki git remote is required.
- Weekday scheduled feedback-loop runs force strict wiki sync behavior (`BANANA_WIKI_STRICT=true`) so wiki clone/push failures fail the run.
- After feedback PR merge, `Train Not-Banana Model` push run automatically persists registry history when `data/not-banana/corpus.json` changes.
- Loop summary: approved feedback -> corpus PR -> human merge -> train + persisted registry-history PR.

Automated SDLC orchestration (incremental PR slices + wiki sync):

- Run workflow `Orchestrate Banana SDLC` to execute multiple incremental automation slices in one run.
- Each increment uses the same GH CLI PR engine as `Orchestrate Triaged Item Pull Request`, but with per-slice commands, commit messages, and PR metadata.
- Inputs support inline `incremental_plan_json` plus global defaults for base branch, labels, reviewers, draft mode, and no-op behavior.
- AI contract preflight validation runs by default (`validate_ai_contracts=true`) and fails fast if `.github` prompt/agent/instruction/skill or wiki-sync contracts drift.
- Wiki layer runs after PR orchestration via `scripts/workflow-sync-wiki.sh` and can push commits to the GitHub wiki repo (or run dry-run for validation).
- Input `wiki_remote_url` defaults to the canonical Banana wiki target (`https://github.com/LahkLeKey/Banana.wiki.git`).
- Default wiki sync now mirrors key `.github` contracts (workflows, instructions, and Copilot instructions) so wiki runbooks stay aligned with automation truth.
- Weekday scheduled SDLC runs force strict wiki sync behavior (`BANANA_WIKI_STRICT=true`) so wiki clone/push failures fail the run.
- Workflow runs on `workflow_dispatch` and on a weekday schedule for unattended incremental automation.
- Default plan includes:
  - feedback ingestion into `data/not-banana/corpus.json`
  - retraining + generated header refresh in `src/native/core/domain/generated/banana_signal_tokens.h`
- Machine-readable summary artifacts are written under `artifacts/sdlc-orchestration/`.

Autonomous cloud self-training cycle (bounded incremental proof of concept):

- Workflow `Orchestrate Autonomous Self-Training Cycle` runs small incremental SDLC slices on weekday schedules and after merge of prior autonomous-cycle PRs.
- The workflow uses bounded resource defaults (`training-profile ci`, `session-mode single`, `max-sessions 1`, and capped feedback ingestion) to keep cloud resource usage minimal.
- Generated PRs use labels `copilot-auto-approve`, `copilot-autonomous-cycle`, and `copilot-bypass-vibe-coded` so Copilot triage can approve and continue the cycle automatically while preserving provenance for vibe-coded integrations.
- Wiki sync is strict and pinned to `https://github.com/LahkLeKey/Banana.wiki.git` for every autonomous run.

Local SDLC dry-run:

```bash
bash scripts/workflow-local-orchestrate-sdlc.sh
```

Direct local SDLC execution with custom plan JSON:

```bash
BANANA_SDLC_INCREMENT_PLAN_JSON='[{"id":"docs","change_command":"python -m py_compile scripts/train-not-banana-model.py","commit_message":"chore(sdlc): validate trainer"}]' \
bash scripts/workflow-orchestrate-sdlc.sh
```

Human-approval merge gate:

- Workflow `Require Human Approval (Automation PRs)` blocks automation PRs until at least one non-bot approval is present.
- Automation PR detection now includes automation labels, known automation branch prefixes, and bot-authored PRs.
- Draft-aware enforcement: draft automation PRs are allowed to stage changes, and the gate is enforced when the PR becomes ready for review.
- Workflow `Copilot Review Triage` tracks review threads from `copilot-pull-request-reviewer`, fails while unresolved findings remain, and posts an up-to-date triage summary on the PR.
- For unresolved Copilot findings, `Copilot Review Triage` now synchronizes one issue per suggestion (label `copilot-suggestion`) so cloud triage orchestration can process remediation incrementally.
- For automation-managed PRs with no Copilot activity yet, `Copilot Review Triage` requests Copilot review and fails in a waiting state until review activity exists.
- When all Copilot findings are triaged, `Copilot Review Triage` applies label `copilot-triage-ready` and submits an approval review from `github-actions[bot]` for automation PRs (or for non-automation PRs labeled `copilot-auto-approve`).
- If unresolved Copilot findings remain, `Copilot Review Triage` applies `copilot-triage-pending` and blocks readiness until triage is complete.
- Labels `copilot-autonomous-cycle` and `copilot-bypass-vibe-coded` enable autonomous continuation: after Copilot triage is ready and bot approval is current, the human-approval check allows bypass and Copilot triage attempts to enable auto-merge.
- Mark `Require Human Approval (Automation PRs)` as required in your protected-branch ruleset so automation merges stay blocked until a human review approves.
- Mark `Copilot Review Triage` as required if you want unresolved Copilot findings to block merge readiness.
- Keep branch settings aligned with `Require a pull request before merging` and reviewer requirements.
- Solo maintainer option: add label `solo-maintainer-bypass` to an owner-authored automation PR to bypass the gate when no second human reviewer exists.
- Safety rule: `solo-maintainer-bypass` fails the check if applied to a PR not opened by the repository owner.

Local workflow containers (Docker Compose):

- Use profile `workflow-local` to run one workflow validation path per container.
- Configure local secrets/defaults once via root `.env`:

```bash
cp .env.example .env
```

- Set at minimum in `.env`:
  - `GH_TOKEN`
  - `BANANA_LOCAL_DRY_RUN=false`
  - `BANANA_REGISTRY_PR_REVIEWERS` and `BANANA_PR_REVIEWERS` (GitHub usernames, comma-separated or `["user1","user2"]`)
- Train + persistence PR path (dry-run orchestration by default):

```bash
bash scripts/compose-local-workflows.sh train
```

- Triaged item PR path with safe no-op change command:

```bash
BANANA_TRIAGE_CHANGE_COMMAND=':' \
bash scripts/compose-local-workflows.sh triaged
```

- Full SDLC incremental PR + wiki sync local path:

```bash
bash scripts/compose-local-workflows.sh sdlc
```

- To run against GitHub for real PR creation, provide `GH_TOKEN` and set `BANANA_LOCAL_DRY_RUN=false`.

## Learn More

- Wiki hub: [Banana Wiki](https://github.com/LahkLeKey/Banana/wiki)
- Onboarding and architecture: `docs/developer-onboarding.md`
- Build/test command index: `.wiki/Build-Run-Test-Commands.md`

> Banana is a prototype for Poly: a polymorphic, multi-platform runtime approach where core behavior starts in C and can be projected into API, desktop, frontend, and mobile delivery channels.
