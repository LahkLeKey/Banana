# Quickstart: Agent Pulse Orchestration

## 1. Validate AI workflow contracts

```bash
cd /c/Github/Banana
python scripts/validate-ai-contracts.py
```

## 2. Run local dry-run orchestration with default plan

```bash
cd /c/Github/Banana
export BANANA_LOCAL_DRY_RUN=true
export BANANA_SDLC_ID=agent-pulse-local
export BANANA_SDLC_INCREMENT_PLAN_MODEL=native-deterministic-agent-pulse
export BANANA_SDLC_INCREMENT_CATALOG_PATH=docs/automation/agent-pulse/autonomous-self-training-default-increments.json
export BANANA_SDLC_CONTINUE_ON_ERROR=true
bash scripts/workflow-orchestrate-sdlc.sh
```

## 3. Optionally render deterministic plan directly

```bash
cd /c/Github/Banana
bash scripts/generate-deterministic-agent-pulse-plan.sh \
	--catalog docs/automation/agent-pulse/autonomous-self-training-default-increments.json \
	--output artifacts/sdlc-orchestration/deterministic-agent-pulse-plan-local.json
```

## 4. Inspect generated orchestration outputs

- Summary JSON: `artifacts/sdlc-orchestration/summary-<run>-attempt-<n>.json`
- Per-increment reports: `artifacts/sdlc-orchestration/increment-*.json`
- Rendered deterministic plan: `artifacts/sdlc-orchestration/deterministic-agent-pulse-plan-*.json`

## 5. Inspect management-facing activity docs

- Index: `docs/automation/agent-pulse/README.md`
- Human-readable guide: `docs/automation/agent-pulse/Human-Reading-Guide.md`
- AI audit index: `docs/automation/agent-pulse/AI-Audit-Trails.md`
- Per-agent roots: `docs/automation/agent-pulse/agents/*/README.md`
- Per-agent run snapshots: `docs/automation/agent-pulse/agents/*/runs/*.md`

## 6. Optional workflow dispatch override

When dispatching `orchestrate-autonomous-self-training-cycle.yml`, provide `incremental_plan_json` only when intentionally overriding the default catalog.
