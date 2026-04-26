# GitHub Actions Workflows (v1 snapshot)

Source: `.github/workflows/`. 10 workflows.

| File | Triggers | Purpose |
| --- | --- | --- |
| `ai-contract-guard.yml` | `pull_request`, `push`, `workflow_dispatch` | Runs `python scripts/validate-ai-contracts.py` and the spec-driven agent attribution smoke test. **Survival critical** — gate against AI surface drift. |
| `compose-ci.yml` | `push`, `workflow_dispatch` | Native C tests + coverage; ASP.NET tests + coverage; uses GH service container `postgres:16` with user/db `cinterop`. Threshold gate via `check-dotnet-coverage-threshold.sh`. |
| `copilot-review-triage.yml` | `pull_request_target` (opened/reopened/synchronize/ready_for_review/converted_to_draft/edited), `pull_request_review`, `workflow_dispatch` (input: `pull_number`) | Permissions: `actions/contents/issues/pull-requests: write`. Triages Copilot reviewer comments into actionable items. |
| `orchestrate-autonomous-self-training-cycle.yml` | `workflow_dispatch` (inputs: `incremental_plan_json`, `wiki_dry_run`, `wiki_push`) | Permissions: `contents/pull-requests: write`. Runs deterministic agent pulse + training plan; opens incremental PRs. |
| `orchestrate-banana-sdlc.yml` | `workflow_dispatch` (inputs: `sdlc_id`, `incremental_plan_json`, `base_branch`, `branch_prefix`) | Wraps `workflow-orchestrate-sdlc.sh`. |
| `orchestrate-not-banana-feedback-loop.yml` | `workflow_dispatch` (inputs: `feedback_path`, `corpus_path`, `status_filter`, `max_entries`) | Wraps `orchestrate-not-banana-feedback-loop.sh`. Default `feedback_path=data/not-banana/feedback/inbox.json`, `corpus_path=data/not-banana/corpus.json`, `status_filter=approved`. |
| `orchestrate-triaged-item-pr.yml` | `workflow_dispatch` (inputs: `triage_id` REQUIRED, `change_command` REQUIRED, `base_branch`, `branch_prefix`, `commit_message`) | Wraps `workflow-orchestrate-triaged-item-pr.sh`. |
| `orchestrate-triage-idea-cloud.yml` | `workflow_dispatch` | Permissions: `actions/contents/issues/pull-requests: write`. Wraps `workflow-triage-idea-cloud.sh`. |
| `require-human-approval.yml` | (gating workflow) | Enforces `BANANA_REQUIRED_HUMAN_REVIEWER`. |
| `train-not-banana-model.yml` | `workflow_dispatch` | Wraps `train-not-banana-model.py` + registry persistence. |

## Conventions captured

- **Schedule-event clauses**: When a job is gated `github.actor != 'github-actions[bot]'`, scheduled runs (cron) execute as the bot and get skipped — include `github.event_name == 'schedule'` to allow them. (Recorded in user memory + spec 013.)
- **YAML quoting**: Quote whole `${{ ... }}` env values when defaults contain `:`, and quote input descriptions containing JSON-like `{key: value}` snippets to avoid parse errors.
- **PR labels**: GH CLI emits warnings for missing labels; pre-create `sdlc`, `feedback-loop`, etc. or scope `BANANA_PR_LABELS` to existing labels.
- **Cron ↔ workflow_dispatch parity**: Several orchestrators support both; v2 should keep schedule + manual dispatch dual-tracked.

## v2 Constraints (spec 013)

- AI Contract Guard MUST migrate to invoke `python .specify/scripts/validate-ai-contracts.py` after the v1 wipe.
- The `expected_prompt_wiki_targets` list (25 entries) lives in the validator and `Auto-Prompts/*.md` wiki pages — both MUST stay in sync via the wiki-consume + wiki-sync survival scripts.
