# Spec-Drain Infrastructure Blocker Handling

**Updated**: 2026-05-02  
**Aligns with**: Constitution Principle XII (Infrastructure Blockers Explicit)

## Overview

The spec-drain automation (`scripts/workflow-spec-drain-loop.sh`) processes specifications sequentially while gracefully handling infrastructure-dependent specs. This document explains how blockers are classified and processed without halting the drainage loop.

## Spec Classification

All specs are classified into three categories by the drain automation:

### 1. Ready for Implementation (Executable)
- **Status**: `**Status**: Ready for implementation` in spec.md
- **Blockers**: No `[INFRASTRUCTURE]` marker in spec.md
- **Processing**: Executed immediately when selected
- **Examples**: Specs 141, 142, 143, 145, 146, 147, 149

**Characteristics**:
- Can be validated and tested without live infrastructure
- Configuration, documentation, and code changes are self-contained
- May reference live services but don't *require* them for implementation

### 2. Infrastructure-Blocked (Deferred)
- **Status**: `**Status**: Ready for implementation` with `[INFRASTRUCTURE]` blocker, OR
- **Status**: `**Status**: Ready for research` 
- **Blockers**: Explicit `[INFRASTRUCTURE]` marker or research-only status
- **Processing**: Skipped during drainage; revisited when prerequisites are met
- **Examples**: Specs 144, 148, 150 (implementation + blocked); Specs 151-155 (research + blocked)

**Characteristics**:
- Require live infrastructure: Vercel project, Railway deployment, PostgreSQL database, production traffic, DNS records
- Cannot be validated without external service interaction
- May need infrastructure setup decisions before implementation can start

### 3. Research-Only (Future Work)
- **Status**: `**Status**: Ready for research` in spec.md
- **Purpose**: Documentation and exploration specs
- **Processing**: Skipped during implementation; executed separately during exploration phases
- **Examples**: Specs 151-155

**Characteristics**:
- Document CLI features, integration points, or decision points
- Require human review before implementation begins
- Useful for identifying gaps and planning follow-up specs

## Infrastructure Blocker Tags

Specs document infrastructure prerequisites using the `[INFRASTRUCTURE]` marker in the Prerequisites section:

```markdown
**Status**: Ready for implementation
**Prerequisites**: Spec 123 (banana.engineer domain live on Vercel)
**Blockers**: [INFRASTRUCTURE] Requires live Vercel project with traffic history
```

### Blocker Categories

| Category | Example Specs | Required Infrastructure | Duration |
|----------|---------------|------------------------|----------|
| **Vercel Setup** | 141-143, 145 | Live Vercel project, domain linked | Until first prod deployment |
| **Railway Setup** | 146, 149 | Railway deployment, API endpoint | Until Railway app deployed |
| **Traffic Data** | 144, 148, 150 | Live production traffic | 7+ days for metrics |
| **Database Ops** | 149, 150 | PostgreSQL via Railway | Live connection |
| **DNS Records** | 155 | Custom domain DNS propagation | 24-48 hours |

## Drainage Loop Behavior

### Spec Selection Algorithm

The `get-next-runnable` command implements this algorithm:

```
For each spec directory in .specify/specs (alphabetically):
  1. Verify spec.md and tasks.md exist
  2. Check if spec is already completed → skip
  3. Check if spec is already failed → skip
  4. Check if Status == "Ready for research" → skip (research phase)
  5. Check if [INFRASTRUCTURE] marker in spec.md → skip (defer)
  6. If all checks pass → Return this spec (runnable)
Return empty string if no runnable specs found (drain exhausted)
```

### Processing Flow

```
┌─────────────────────────────────────┐
│ Get next runnable spec              │
│ (filters out blockers)              │
└────────┬────────────────────────────┘
         │
         ├──→ Spec executable?
         │    YES: Proceed to implementation
         │    NO (blocker): Mark as deferred
         │
         ├──→ Implementation successful?
         │    YES: Mark completed, continue
         │    NO: Count failure, retry or skip
         │
         └──→ Budget exhausted or all specs processed?
              YES: Stop drainage
              NO: Get next runnable spec
```

## Examples: Processing Specs 141-155

### Phase 1: Infrastructure Available (Vercel + Railway deployed)
```
1. Spec 141 (www-domain-redirect) → COMPLETED
2. Spec 142 (deploy-hooks) → COMPLETED
3. Spec 143 (logs-inspection) → COMPLETED
4. Spec 144 (metrics-observability) → SKIPPED [needs 7+ days traffic]
5. Spec 145 (edge-middleware) → COMPLETED
6. Spec 146 (health-checks) → COMPLETED
7. Spec 147 (performance-budget) → COMPLETED
8. Spec 148 (error-monitoring) → SKIPPED [needs error history]
9. Spec 149 (rate-limit-verification) → COMPLETED
10. Spec 150 (database-performance) → SKIPPED [needs query patterns]
11-15. Specs 151-155 → SKIPPED [research status]
```

### Phase 2: Traffic Data Collected (7+ days later)
```
Resume drainage with environment variable set:
BANANA_DRAIN_INFRASTRUCTURE_READY=true

1. Spec 144 (metrics) → COMPLETED [now has traffic data]
2. Spec 148 (error-rate) → COMPLETED [now has error history]
3. Spec 150 (database-perf) → COMPLETED [now has query patterns]
```

### Phase 3: Infrastructure Decisions Complete
```
Once decisions made on blob storage, cron jobs, rollback strategy:

1. Specs 151-155 transition from "Ready for research" to "Ready for implementation"
2. Drainage resumes and completes specs 151-155
3. Full drain exhaustion achieved
```

## Constitution Alignment

### Principle XII: Infrastructure Blockers Explicit
> Infrastructure prerequisites are documented in spec Prerequisites/Blockers, not scattered across runbooks. Blocked specs don't prevent downstream work.

**Implementation**:
- ✅ Each blocked spec has explicit `[INFRASTRUCTURE]` marker
- ✅ Prerequisites documented in spec.md
- ✅ Drainage automation skips blockers but continues (doesn't fail)
- ✅ Downstream specs are not affected by upstream blockers

### Principle XI: Spec-Driven Infrastructure Discovery
> Infrastructure-as-code specs are created before implementation. Mark as "Ready for research" or "Ready for implementation" to guide prioritization.

**Implementation**:
- ✅ Specs 141-155 created via CLI discovery
- ✅ Status markers used to signal infrastructure requirements
- ✅ Specs 151-155 marked "Ready for research" for exploration phase

### Principle X: Observable Infrastructure as Code
> Infrastructure config is versioned in specs, documented in runbooks, validated in CI/CD.

**Implementation**:
- ✅ Vercel, Railway, DNS configs documented in spec tasks
- ✅ Health checks and diagnostics endpoints mandatory (specs 146, 147)
- ✅ CI gates enforce performance budgets and error rate thresholds

## State File Format

The spec-drain state checkpoint preserves the drainage state:

```json
{
  "schema_version": 1,
  "mode": "spec-drain",
  "run_id": "autonomous-drain-001",
  "run_attempt": 1,
  "status": "running",
  "stop_reason": null,
  "completed_specs": ["000-api-rehydration", "001-cli-bootstrap", ...],
  "failed_specs": [
    {"spec_id": "050-invalid-spec", "reason": "quality_gate"}
  ]
}
```

## Environment Variables

Control drainage behavior with:

```bash
# Core drainage config
BANANA_DRAIN_STATE_PATH=artifacts/spec-drain-state.json
BANANA_DRAIN_MAX_FAILURES=5                              # Stop after N unique failures
BANANA_DRAIN_MAX_RETRIES=2                               # Retry failed spec N times
BANANA_DRAIN_DRY_RUN=true                                # Simulate without changes

# Infrastructure readiness (future use)
BANANA_DRAIN_INFRASTRUCTURE_READY=true                   # Process infrastructure specs
BANANA_DRAIN_RESEARCH_PHASE=true                         # Process "Ready for research" specs

# Emergency controls
BANANA_AUTONOMY_STOP=true                                # Kill switch: halt drainage
```

## Operational Runbooks

### Monitor Drainage Progress
```bash
# Check current state
jq . artifacts/spec-drain-state.json

# Get summary
bash scripts/workflow-spec-drain-state.sh get-summary artifacts/spec-drain-state.json
```

### Manually Skip a Blocked Spec
```bash
# Mark spec as deferred without failing
bash scripts/workflow-spec-drain-state.sh mark-failed \
  artifacts/spec-drain-state.json \
  141-vercel-redirects-www-domain \
  "infrastructure_not_ready"
```

### Transition Research Spec to Implementation
```bash
# Edit spec.md to change status
# From: **Status**: Ready for research
# To:   **Status**: Ready for implementation
# Remove or update [INFRASTRUCTURE] marker as needed

# Re-initialize drainage to pick up changes
bash scripts/workflow-spec-drain-state.sh init \
  artifacts/spec-drain-state.json \
  autonomous-drain \
  2
```

## Next Steps

1. **Short-term**: Run `workflow-orchestrate-sdlc.sh` to execute specs 141-150 (no infrastructure blockers)
2. **Medium-term**: After 7+ days production traffic, re-run drainage to complete specs 144, 148, 150
3. **Long-term**: Review specs 151-155 for infrastructure decisions, update status markers, and trigger final drainage phase
