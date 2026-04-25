# Agent Pulse Orchestration Index

This directory provides a management-friendly deep-dive map for autonomous SDLC activity.

## Why This Exists

- Show clear ownership for autonomous increments by agent lane.
- Make it easy to inspect what each agent did in each workflow run.
- Keep visibility artifacts in-repo, versioned, and searchable.

## Followable Agent Identity Map

Canonical source of truth: `docs/automation/agent-pulse/agent-identities.json`

| Agent Slug | Icon | Display Name | Follow Target | Snapshot Root |
| --- | --- | --- | --- | --- |
| banana-classifier-agent | Ba | Banana Classifier Agent | [agent pulse lane](agents/banana-classifier-agent/) | `docs/automation/agent-pulse/agents/banana-classifier-agent/` |
| api-pipeline-agent | Ap | API Pipeline Agent | [agent pulse lane](agents/api-pipeline-agent/) | `docs/automation/agent-pulse/agents/api-pipeline-agent/` |
| csharp-api-agent | Cs | CSharp API Agent | [agent pulse lane](agents/csharp-api-agent/) | `docs/automation/agent-pulse/agents/csharp-api-agent/` |
| native-core-agent | Nc | Native Core Agent | [agent pulse lane](agents/native-core-agent/) | `docs/automation/agent-pulse/agents/native-core-agent/` |
| react-ui-agent | Ru | React UI Agent | [agent pulse lane](agents/react-ui-agent/) | `docs/automation/agent-pulse/agents/react-ui-agent/` |
| electron-agent | El | Electron Agent | [agent pulse lane](agents/electron-agent/) | `docs/automation/agent-pulse/agents/electron-agent/` |
| mobile-runtime-agent | Mo | Mobile Runtime Agent | [agent pulse lane](agents/mobile-runtime-agent/) | `docs/automation/agent-pulse/agents/mobile-runtime-agent/` |
| integration-agent | Au | Integration Agent | [github.com/integration-agent](https://github.com/integration-agent) | `docs/automation/agent-pulse/agents/integration-agent/` |
| workflow-agent | Wf | Workflow Agent | [agent pulse lane](agents/workflow-agent/) | `docs/automation/agent-pulse/agents/workflow-agent/` |

## Why Default Pulse Icons Still Appear

Pulse avatars are not controlled by this table. GitHub Pulse resolves avatars from commit author identity that maps to a real GitHub account.

- If a contributor row has `login: null` and `avatar_url: null` from the contributors API, Pulse renders a default gray icon.
- `display_name` and `icon` in `agent-identities.json` are management metadata and do not force a Pulse avatar.
- `integration-agent` currently resolves because it is mapped to an actual GitHub login with an avatar.

Quick verification command:

- `gh api 'repos/LahkLeKey/Banana/contributors?per_page=100&anon=1' --jq '.[] | {login: .login, name: .name, email: .email, avatar_url: .avatar_url}'`

To get non-default icons for another lane, map it in `agent-identities.json` to a real GitHub user login (and its commit email alias) that has a profile avatar.

## Snapshot Format

Each increment writes one run snapshot under:

- `docs/automation/agent-pulse/agents/<agent>/runs/run-<run-id>-attempt-<attempt>-<increment>.md`

Snapshots include intent summary, workflow metadata, run link, actor, ref, and SHA.

## Default Increment Catalog

- `docs/automation/agent-pulse/autonomous-self-training-default-increments.json`

The autonomous self-training workflow uses that catalog by default and only falls back to `incremental_plan_json` when the workflow input is explicitly provided.

Current default catalog behavior:

- The catalog is source-first and workflow-first by default.
- Primary increments are classifier refresh + generated native header sync and Banana API coverage denominator sync.
- Markdown-only pulse snapshot increments are intentionally excluded from the default catalog to reduce non-impact PR churn.

## Native Deterministic Model

- Model source: `src/native/core/domain/banana_agent_pulse_model.c`
- Model CLI: `src/native/core/domain/banana_agent_pulse_model_cli.c`
- Plan renderer: `scripts/generate-deterministic-agent-pulse-plan.sh`

When `BANANA_SDLC_INCREMENT_PLAN_MODEL=native-deterministic-agent-pulse` and no manual override JSON is provided, orchestration renders a deterministic plan from the base catalog using the native C lane model. The model controls per-agent execution order and pulse repetition counts, which increases repeatable multi-agent commit activity while preserving clear ownership per lane.
