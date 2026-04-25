# Tasks: Backlog Risk-Value Prioritization

**Input**: Design documents from `.specify/specs/003-backlog-risk-value-prioritization/`  
**Prerequisites**: `plan.md`, `spec.md`, `research.md`, `data-model.md`, `contracts/backlog-risk-value-scorecard.md`, `quickstart.md`

**Tests**: Deterministic replay checks and contract validation are required.

## Phase 1: Setup

- [ ] T001 Create backlog prioritization docs roots under `docs/automation/backlog/`
- [ ] T002 Create script scaffold `scripts/prioritize-backlog-risk-value.py`
- [ ] T003 Create artifact naming convention for snapshots under `artifacts/sdlc-orchestration/`

## Phase 2: Foundational Scoring Contract

- [ ] T004 Implement issue snapshot ingestion and normalization from GitHub issue metadata in `scripts/prioritize-backlog-risk-value.py`
- [ ] T005 Implement dependency graph extraction from issue body references in `scripts/prioritize-backlog-risk-value.py`
- [ ] T006 Implement score policy loading and validation against `.specify/specs/003-backlog-risk-value-prioritization/contracts/backlog-risk-value-scorecard.md`

## Phase 3: User Story 1 - High-Value Execution Slate (Priority: P1)

**Goal**: Emit deterministic ranked shortlist of high-value ready items.

**Independent Test**: Re-run prioritization with the same inputs and confirm ordering remains stable.

- [ ] T007 [US1] Implement value gain and risk reduction scoring dimensions
- [ ] T008 [US1] Implement dependency unlock bonus and effort/risk penalties
- [ ] T009 [US1] Implement deterministic tie-break ordering and immediate top-5 slate output

## Phase 4: User Story 2 - Dependency-Safe Sequencing (Priority: P2)

**Goal**: Provide owner-aware, dependency-safe execution order.

**Independent Test**: Verify immediate queue excludes blocked items unless their blockers are included first.

- [ ] T010 [US2] Implement blocked/deferred queue separation using dependency graph
- [ ] T011 [US2] Map recommended items to owner helper agents from `agent:*` labels
- [ ] T012 [US2] Emit validation-owner and execution-order hints per selected item

## Phase 5: User Story 3 - Auditable Priority Decisions (Priority: P3)

**Goal**: Preserve explainable score history and policy-change transparency.

**Independent Test**: Compare two snapshots and confirm visible score-delta reasons.

- [ ] T013 [US3] Emit AI-audit snapshot JSON with full score breakdown and blocker traces
- [ ] T014 [US3] Emit human-readable summary markdown with rationale and next actions
- [ ] T015 [US3] Add policy version and score-delta reporting between runs

## Phase 6: Polish & Governance

- [ ] T016 Update orchestration/agent docs to route backlog prioritization requests to `value-risk-prioritization-agent`
- [ ] T017 Run `python scripts/validate-ai-contracts.py`
- [ ] T018 Run deterministic replay validation and verify unchanged ranking for identical input snapshots
