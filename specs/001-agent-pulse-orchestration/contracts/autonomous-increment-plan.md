# Contract: Autonomous Increment Plan

## Purpose

Define the JSON shape consumed by `scripts/workflow-orchestrate-sdlc.sh` through:

- `BANANA_SDLC_INCREMENT_PLAN_JSON` (inline override), or
- `BANANA_SDLC_INCREMENT_PLAN_PATH` (repository-managed default file)

Native deterministic rendering can guide default plan generation when:

- `BANANA_SDLC_INCREMENT_PLAN_MODEL=native-deterministic-agent-pulse`
- `BANANA_SDLC_INCREMENT_CATALOG_PATH` points at the base catalog JSON

## Accepted Payload Shapes

1. JSON array of increment objects
2. JSON object with `increments` array

## Increment Object Schema

- `id` (string, required): unique increment key.
- `change_command` (string, required): command evaluated by triage orchestrator.
- `commit_message` (string, optional): commit title; fallback is `chore(sdlc): <id>`.
- `pr_title` (string, optional): pull request title.
- `pr_body` (string, optional): pull request body.
- `labels` (string, optional): comma-delimited labels; default labels apply when omitted.
- `reviewers` (string, optional): comma-delimited reviewer list.
- `draft_pr` (string, optional): `true` or `false`.
- `base_branch` (string, optional): target base branch.
- `branch_prefix` (string, optional): branch prefix override.

## Behavioral Guarantees

- Duplicate increment IDs are rejected.
- Empty or missing `change_command` fails plan parsing.
- When `incremental_plan_json` input is provided, it takes precedence over the default plan path.
- PR contributor attribution is derived from `agent:*` labels by `scripts/workflow-orchestrate-triaged-item-pr.sh`.
- When native deterministic model mode is enabled and no inline override is provided, SDLC orchestration renders a deterministic plan before parsing increments.

## Native Deterministic Lane Model Schema

`banana_agent_pulse_model_cli` emits:

- `model` (string): deterministic model identifier.
- `lanes` (array): lane rows with:
	- `agent` (string)
	- `priority` (integer, strictly descending)
	- `deterministic_snapshots` (integer >= 1)
	- `focus` (string)

## Management Visibility Requirements for Default Catalog

- Each default increment should contain exactly one `agent:*` ownership marker.
- Default catalog should include multiple agent lanes, not only one agent.
- Each increment should carry a clear intent summary in `pr_body` for management readability.
- Default catalog should include a documentation ownership lane (`agent:technical-writer-agent`) when wiki/index audience split updates are required.
- Management-visible outputs should keep separate human-readable and AI-audit entry points linked from one shared navigation section.
