# Research: C Upstream Value Reassessment SPIKE

**Branch**: `feature/006-c-upstream-value-spike` | **Phase**: 0 | **Date**: 2026-04-26

## R-01: Candidate prioritization method

**Question**: What scoring model should rank native C candidates for upstream value decisions?

**Decision**: Use a normalized weighted scorecard per candidate:

- `value_gain` (0-5)
- `risk_reduction` (0-5)
- `dependency_unlock` (0-5)
- `delivery_effort` (0-5, inverse contribution)
- `confidence` (0-5)

Composite score:

`score = 0.35*value_gain + 0.30*risk_reduction + 0.20*dependency_unlock + 0.10*confidence - 0.15*delivery_effort`

**Rationale**: Balances business value, reliability improvements, sequencing impact, and delivery cost while keeping selection transparent.

**Alternatives considered**:
- Pure value-only ranking: rejected because it ignores risk and dependency unlock.
- Binary high/low triage: rejected because it lacks tie-break precision.

## R-02: Candidate inventory coverage

**Question**: What minimum inventory scope is sufficient for this SPIKE?

**Decision**: Assess at least 5 candidate opportunities across native core, wrapper/interop contract, and operational reliability surfaces.

**Rationale**: Enough breadth to avoid local optimization while remaining small enough for one review session.

**Alternatives considered**:
- 3 candidates: rejected as too narrow and bias-prone.
- 10+ candidates: rejected as excessive for a quick follow-up SPIKE.

## R-03: Upstream impact mapping format

**Question**: How should cross-domain impact be captured for each top candidate?

**Decision**: Each top candidate includes an impact matrix with:

- impacted domains (`native`, `aspnet`, `typescript-api`, `react/ui`, `tests/automation`)
- contract surfaces affected (status codes, payload shape, route surface, env/runtime contract)
- required validation lanes (`tests/native`, `tests/unit`, `tests/e2e`, `src/typescript/api` tests)

**Rationale**: Ensures follow-up stories are implementation-ready with explicit ownership and validation expectations.

**Alternatives considered**:
- Freeform narrative only: rejected due ambiguity.
- Domain-only tags without contract/validation detail: rejected due incomplete handoff quality.

## R-04: Follow-up story readiness threshold

**Question**: How many top candidates should receive full handoff packets in this SPIKE?

**Decision**: Produce readiness packets for top 3 ranked candidates.

**Rationale**: Matches spec success criteria and supports immediate next-sprint pick without overproducing artifacts.

**Alternatives considered**:
- Top 1 only: rejected as too brittle if priority changes.
- Full packets for all candidates: rejected due low ROI for lower-ranked items.

## R-05: Validation approach for a planning-only SPIKE

**Question**: What quality gate applies when no runtime code changes are made?

**Decision**: Use checklist completion plus internal consistency checks across `spec.md`, `plan.md`, `research.md`, `data-model.md`, and `contracts/`.

**Rationale**: Appropriate gate for analysis artifacts while preserving Spec Kit traceability requirements.

**Alternatives considered**:
- Running full code test suites for this planning step: rejected as unnecessary for artifact-only changes.
