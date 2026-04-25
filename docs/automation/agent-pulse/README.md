# Agent Pulse Orchestration Index

This directory provides a management-friendly deep-dive map for autonomous SDLC activity.

## Why This Exists

- Show clear ownership for autonomous increments by agent lane.
- Make it easy to inspect what each agent did in each workflow run.
- Keep visibility artifacts in-repo, versioned, and searchable.

## Agent Deep-Dive Map

| Agent | Primary Focus | Snapshot Root |
| --- | --- | --- |
| banana-classifier-agent | Not-banana feedback and model-training automation | `docs/automation/agent-pulse/agents/banana-classifier-agent/` |
| api-pipeline-agent | ASP.NET pipeline and controller-service workflow lane visibility | `docs/automation/agent-pulse/agents/api-pipeline-agent/` |
| csharp-api-agent | C# API service and interop lane visibility | `docs/automation/agent-pulse/agents/csharp-api-agent/` |
| native-core-agent | Native core behavior lane visibility | `docs/automation/agent-pulse/agents/native-core-agent/` |
| react-ui-agent | React UI lane visibility | `docs/automation/agent-pulse/agents/react-ui-agent/` |
| electron-agent | Electron runtime lane visibility | `docs/automation/agent-pulse/agents/electron-agent/` |
| mobile-runtime-agent | Mobile runtime lane visibility | `docs/automation/agent-pulse/agents/mobile-runtime-agent/` |
| integration-agent | Cross-domain integration and validation lane visibility | `docs/automation/agent-pulse/agents/integration-agent/` |
| workflow-agent | Workflow and CI governance lane visibility | `docs/automation/agent-pulse/agents/workflow-agent/` |

## Snapshot Format

Each increment writes one run snapshot under:

- `docs/automation/agent-pulse/agents/<agent>/runs/run-<run-id>-attempt-<attempt>-<increment>.md`

Snapshots include intent summary, workflow metadata, run link, actor, ref, and SHA.

## Default Increment Catalog

- `docs/automation/agent-pulse/autonomous-self-training-default-increments.json`

The autonomous self-training workflow uses that catalog by default and only falls back to `incremental_plan_json` when the workflow input is explicitly provided.

## Native Deterministic Model

- Model source: `src/native/core/domain/banana_agent_pulse_model.c`
- Model CLI: `src/native/core/domain/banana_agent_pulse_model_cli.c`
- Plan renderer: `scripts/generate-deterministic-agent-pulse-plan.sh`

When `BANANA_SDLC_INCREMENT_PLAN_MODEL=native-deterministic-agent-pulse` and no manual override JSON is provided, orchestration renders a deterministic plan from the base catalog using the native C lane model. The model controls per-agent execution order and pulse repetition counts, which increases repeatable multi-agent commit activity while preserving clear ownership per lane.
