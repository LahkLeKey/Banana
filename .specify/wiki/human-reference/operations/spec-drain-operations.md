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

These specs are **skipped without failure** — they don't halt downstream work.

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

Expected signal:

```json
{
  "stop_reason": "policy_blocked"
}
```

Remediation:

1. Provide `BANANA_DRAIN_PLAN_PATH`.
2. Ensure the JSON includes an entry for each runnable spec.
3. Ensure each entry contains at least `spec_id` and `change_command`.

### Spec Stuck as Failed

If a spec fails repeatedly:

1. Check the failure reason in the state file:
   ```bash
   jq '.failed_specs[] | select(.spec_id == "099-spec-name")' \
     artifacts/spec-drain-state.json
   ```

2. Inspect the execution log:
   ```bash
   ls -la artifacts/orchestration/spec-drain/*/099-spec-name/
   cat artifacts/orchestration/spec-drain/*/099-spec-name/execution.log
   ```

3. Manual retry (resets failure count for that spec):
   ```bash
   bash scripts/workflow-spec-drain-state.sh mark-failed \
     artifacts/spec-drain-state.json \
     "099-spec-name" \
     "manual_retry_requested"
   ```

### Drainage Halted with "budget_exceeded"

Too many failed specs. Check what failed:

```bash
jq '.failed_specs | group_by(.spec_id) | length' \
  artifacts/spec-drain-state.json
```

If failures are due to infrastructure blockers (not actual errors), reset the budget counter by re-initializing the state and explicitly marking blockers as deferred:

```bash
bash scripts/workflow-spec-drain-state.sh init \
  artifacts/spec-drain-state.json \
  "resumed-after-budget-review" \
  2
```

### Spec Missing from Drainage

Check if spec meets runnable criteria:

```bash
# 1. Verify spec.md and tasks.md exist
ls .specify/specs/ABC-*/spec.md .specify/specs/ABC-*/tasks.md

# 2. Check status in spec.md
grep "Status:" .specify/specs/ABC-*/spec.md

# 3. Check for infrastructure blockers
grep "\[INFRASTRUCTURE\]" .specify/specs/ABC-*/spec.md

# If "Ready for research": Spec is intentionally skipped (research phase)
# If "[INFRASTRUCTURE]" present: Spec is intentionally skipped (infrastructure not ready)
# Otherwise: Spec should be discovered; check drainage logs
```

## Health Checks

### Daily: Verify State Integrity

```bash
# Check state file is valid JSON
jq empty artifacts/spec-drain-state.json && echo "OK" || echo "CORRUPT"

# Count progress
echo "Completed: $(jq '.completed_specs | length' artifacts/spec-drain-state.json)"
echo "Failed: $(jq '.failed_specs | length' artifacts/spec-drain-state.json)"
echo "Status: $(jq -r '.status' artifacts/spec-drain-state.json)"
```

### Weekly: Review Blockers

```bash
# List all blocked specs
for spec in .specify/specs/*/spec.md; do
  if grep -q "\[INFRASTRUCTURE\]" "$spec"; then
    dirname "$spec" | xargs basename
  fi
done
```

### Monthly: Assess Infrastructure Readiness

```bash
# Which infrastructure categories have blockers?
for spec in .specify/specs/*/spec.md; do
  if grep -q "Vercel" "$spec" && grep -q "\[INFRASTRUCTURE\]" "$spec"; then
    echo "VERCEL: $(dirname "$spec" | xargs basename)"
  fi
done
```

## Related Documentation

- [Constitution Principle XII: Infrastructure Blockers Explicit](./../governance/constitution.md)
- [Spec-Drain Infrastructure Blockers Handler](../../../docs/automation/spec-drain-infrastructure-blockers.md)
- [Spec Kit Workflow](./../governance/spec-kit-workflow.md)
