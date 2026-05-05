# Spec-Drain Operations Runbook

**Updated**: 2026-05-02  
**Type**: Infrastructure Automation Documentation  
**Audience**: DevOps, Platform Engineers, Automation Operators

## Quick Start

Monitor and control the autonomous spec-drain process:

```bash
# Check drainage progress
jq . artifacts/spec-drain-state.json

# Show summary
bash scripts/workflow-spec-drain-state.sh get-summary \
  artifacts/spec-drain-state.json

# View evidence logs
ls -la artifacts/orchestration/spec-drain/*/*/result.json
```

Real execution requires an explicit per-spec execution manifest. Dry-run mode validates discovery and quality gates without it, but non-dry-run execution stops with `policy_blocked` unless `BANANA_DRAIN_PLAN_PATH` points to a JSON file that provides a `change_command` for each runnable spec.

```bash
# Preview drain selection only
BANANA_DRAIN_DRY_RUN=true \
bash scripts/workflow-spec-drain-loop.sh

# Real execution requires a plan manifest
BANANA_DRAIN_PLAN_PATH=docs/automation/spec-drain-plan.example.json \
BANANA_DRAIN_DRY_RUN=false \
bash scripts/workflow-spec-drain-loop.sh
```

### Compute-Efficient Scan Pattern

Prefer checkpoint resume and compact summaries before launching another full scan:

```bash
# Resume from existing checkpoint first
BANANA_DRAIN_STATE_PATH=artifacts/sdlc-orchestration/spec-drain-state-all.json \
BANANA_DRAIN_DRY_RUN=true \
bash scripts/workflow-spec-drain-loop.sh

# Read compact summary for planning instead of re-scanning
jq '{status, stop_reason, completed_count, failed_count}' \
  artifacts/orchestration/spec-drain-all/local-run/drain-summary.json
```

Run a full fresh scan only when spec inventory shape changed or checkpoint state is missing/corrupt.

### Close With Deferred Ledger

When the scan reaches exhaustion but blocked/research specs remain, publish an accounted summary instead of re-running full scan loops:

```bash
python - <<'PY'
import json
from pathlib import Path

state = json.loads(Path('artifacts/sdlc-orchestration/spec-drain-state-all-accounted.json').read_text())
all_specs = [x.name for x in Path('.specify/specs').iterdir() if x.is_dir() and (x/'spec.md').exists() and (x/'tasks.md').exists()]
completed = set(state.get('completed_specs', []))
deferred = {f.get('spec_id') for f in state.get('failed_specs', [])}

print(json.dumps({
  'completed_count': len(completed),
  'deferred_count': len(deferred),
  'accounted_total': len(completed) + len(deferred),
  'all_specs_with_tasks': len(all_specs),
  'unaccounted_count': len(set(all_specs) - completed - deferred)
}, indent=2))
PY
```

Expected end-state: `unaccounted_count` is `0`.

## Infrastructure Blocker States

The drainage process classifies specs into executable and deferred categories:

### Executable (Will Process Immediately)
- **Status**: `Ready for implementation`
- **Prerequisites**: No `[INFRASTRUCTURE]` marker
- **Examples**: Specs 141-143, 145-147, 149 from latest discovery batch

These specs execute on next drainage cycle without infrastructure setup.

### Deferred (Will Skip Until Prerequisites Met)
- **Status**: `Ready for research` OR has `[INFRASTRUCTURE]` blocker
- **Prerequisites**: Requires live Vercel/Railway/DNS/production metrics
- **Examples**: Specs 144, 148, 150, 151-155 from latest discovery batch

These specs are **skipped without failure** - they don't halt downstream work.

## Common Operations

### Monitor Current Drainage

```bash
# Show completed specs
jq '.completed_specs | length' artifacts/spec-drain-state.json

# Show failed specs (with reasons)
jq '.failed_specs[] | {id: .spec_id, reason}' artifacts/spec-drain-state.json

# Show stop reason (if any)
jq '.stop_reason' artifacts/spec-drain-state.json
```

### Transition a Spec from Research to Implementation

When infrastructure decisions are complete, update the spec status:

```bash
# 1. Edit the spec.md file
nano .specify/specs/151-vercel-blob-storage/spec.md

# Change from:
# **Status**: Ready for research
# To:
# **Status**: Ready for implementation
# Remove or update [INFRASTRUCTURE] blocker marker

# 2. Commit changes
git add .specify/specs/151-*/spec.md
git commit -m "Update spec 151 status: research complete, ready for implementation"

# 3. Reset drainage to pick up change
bash scripts/workflow-spec-drain-state.sh set-stop \
  artifacts/spec-drain-state.json \
  "resume_after_infrastructure_decision"

# 4. Next drainage cycle will process the updated spec
```

### Handle a Blocked Spec That Will Never Execute

If a spec is no longer relevant, mark it permanently failed:

```bash
bash scripts/workflow-spec-drain-state.sh mark-failed \
  artifacts/spec-drain-state.json \
  "150-database-performance-monitoring" \
  "out_of_scope_decision_made"
```

### Emergency Stop Drainage

Use the kill switch in critical situations:

```bash
BANANA_AUTONOMY_STOP=true bash scripts/workflow-spec-drain-loop.sh
```

Or set in GitHub Actions environment to halt in-flight runs.

### Resume Drainage from Checkpoint

Drainage **automatically resumes** from saved state. To explicitly resume:

```bash
BANANA_DRAIN_PLAN_PATH=path/to/spec-drain-plan.json \
bash scripts/workflow-spec-drain-loop.sh
```

The loop reads the state file and continues from the last processed spec.

## Infrastructure Readiness Timeline

### Today (No Infrastructure)
```
✓ Specs 141-143: Basic config (Vercel redirects, deploy hooks, logs inspection)
✓ Specs 145-147: Documentation + code (edge middleware, health checks, budgets)
✓ Spec 149: Test code (rate limit verification without live API)
✗ Specs 144, 148, 150: Require traffic/error data (deferred)
✗ Specs 151-155: Research phase (deferred)
```

**Action**: Run drainage to complete executable specs.

### Week 1-2 (Vercel + Railway Live)
```
✓ All specs from "Today" still completed
✓ Specs 141-149: All processing complete
✗ Specs 144, 148, 150: Still waiting for traffic/error history
✗ Specs 151-155: Still research phase
```

**Action**: No new executables; continue monitoring metrics.

### Week 2-3 (Production Traffic Data)
```
✓ Specs 144, 148, 150: Now have 7+ days traffic, can complete
✓ Specs 146-147: Performance budgets validated by live metrics
```

**Action**: Update environment variable and resume drainage:
```bash
BANANA_DRAIN_PLAN_PATH=path/to/spec-drain-plan.json \
bash scripts/workflow-spec-drain-loop.sh
```

### Week 3+ (Infrastructure Decisions)
```
✓ Specs 151-155: Infrastructure review complete, status updated
✓ All specs 1-160: Ready for implementation or explicitly skipped
```

**Action**: Final drainage cycle completes all remaining specs.

## Troubleshooting

### Drainage Stops with "policy_blocked"

The loop reached a runnable spec in real mode, but no execution plan was provided for it.

```bash
jq . artifacts/orchestration/spec-drain/local-run/drain-summary.json
```
