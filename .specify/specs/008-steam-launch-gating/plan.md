# Implementation Plan: Steam Launch Gating

**Branch**: `feat/session-room-prod-character-onboarding` | **Date**: 2026-05-24 | **Spec**: `.specify/specs/008-steam-launch-gating/spec.md`

**Input**: Feature specification from `.specify/specs/008-steam-launch-gating/spec.md`

## Summary

Implement a hard native launch gate that requires valid Steam identity and linked production-account good-standing verification for production package launches, with explicit environment-mode behavior, a deny-safe fallback/override policy, API contracts for verification/status, and clear UX remediation paths for blocked users.

## Technical Context

**Language/Version**: Native C/C++ runtime (CMake v3 native tree), TypeScript 5.x API on Bun

**Primary Dependencies**: Native launch/runtime boundary (`src/native`), Fastify API contracts (`src/typescript/api`), existing web account linking flow

**Storage**: Authoritative production account and linkage status from existing account service data store (PostgreSQL-backed via API runtime contracts)

**Testing**: Native launch-path tests, API contract/unit tests (`bun test`), integration matrix for allow/deny and retry/remediation outcomes

**Target Platform**: Windows Steam production package and local development channels

**Project Type**: Cross-domain native + API + web-remediation contract feature in monorepo

**Performance Goals**: Launch verification decision available within interactive launch UX thresholds; retries reflect updated status on next attempt after transient recovery (SC-005)

**Constraints**: Production gating cannot be bypassed by environment overrides; stale or unavailable verification must deny in production; decision reasons must be auditable and user-actionable

**Scale/Scope**: Native launch checkpoints, ABI/API verification decisioning, blocked-launch UX state model, and web registration/linking remediation contracts

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- [x] Player trust and disclosure alignment verified. Production access behavior is explicit and reason-coded; no deceptive bypass behavior is allowed.
- [x] Storefront governance artifacts identified when public Steam copy is affected. If storefront text lacks account prerequisites, follow-on contract/checklist artifacts are required before release.
- [x] Cross-domain contracts mapped for touched layers (native/API/client/runtime) and required docs are queued in-scope.
- [x] Quality gates defined with measurable checks for deterministic allow/deny behavior, remediation, and failure handling.
- [x] Reproducible delivery path identified for target runtime channels with evidence outputs listed in quickstart.

**Post-Design Re-check**: Pass. Phase 0 research resolved policy decisions and failure semantics; Phase 1 data model and contracts encode explicit non-bypass production rules with no unresolved constitution conflicts.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/008-steam-launch-gating/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   ├── launch-gate-api-contract.md
│   ├── environment-mode-policy-contract.md
│   ├── blocked-launch-ux-contract.md
│   └── web-account-linking-remediation-contract.md
└── tasks.md
```

### Source Code (repository root)

```text
src/native/
├── engine/
├── include/
└── scaffold/

src/typescript/api/
└── src/

src/typescript/react/
└── src/
```

**Structure Decision**: Keep implementation in existing domain roots (`src/native`, `src/typescript/api`, and web remediation surfaces) to preserve controller -> service -> pipeline -> native interop boundaries and avoid unnecessary structural churn.

## Storefront Deliverables

- Public claim updates are conditionally required if release/store copy currently omits Steam-linked account prerequisites.
- Required follow-on artifacts when copy changes are in-scope:
  - `contracts/steam-storefront-copy.md`
  - `contracts/steam-storefront-assets.md`
  - `checklists/steam-storefront-readiness.md`
  - `checklists/steam-uat-readiness.md`

## Phase Plan

### Phase 0: Research (complete)

- Resolved hard-gate location, environment-mode taxonomy, deny-safe fallback/override rules, API reason taxonomy, UX blocked-state model, and outage/stale-session behavior.
- Output: `research.md`

### Phase 1: Design and Contracts (complete)

- Captured entities and validation/state rules in `data-model.md`.
- Defined verification/status API, environment mode policy, blocked-launch UX state contract, and web remediation handoff under `contracts/`.
- Authored validation and evidence workflow in `quickstart.md`.
- Updated agent context pointer in `.github/copilot-instructions.md` to this feature plan.

### Phase 2: Task Planning Inputs (prepared for `/speckit.tasks`)

| Slice | Goal | Primary Areas | Validation Focus |
|-------|------|---------------|------------------|
| S1 | Add native launch checkpoint and mode-resolution policy | `src/native` launch/runtime paths | Production hard-deny and dev-mode policy tests |
| S2 | Implement API verify/status/retry endpoints with reason taxonomy | `src/typescript/api/src/routes` + services | Contract tests for allow/deny/retry semantics |
| S3 | Integrate blocked-launch UX state mapping | client/runtime + web-facing messaging layer | Reason-code to action mapping and clarity checks |
| S4 | Implement web remediation ticket lifecycle and re-verify flow | API + web linking integration points | Link completion then fresh verification required |
| S5 | Harden outage/offline/stale-session behavior | native/API retry and timeout handling | Deterministic failure-mode matrix and audit events |

## Validation Strategy

1. API baseline: `cd src/typescript/api && bun test`.
2. Native launch matrix: verify allow/deny outcomes for good-standing, unlinked, suspended, stale, offline, and upstream-unavailable conditions.
3. Mode policy checks: prove production ignores weakening overrides; non-production behavior remains explicitly flagged/audited.
4. Remediation checks: complete web registration/linking and verify next launch requires fresh verification before allow.
5. Evidence capture paths in `quickstart.md` populate feature-local and artifacts evidence files.

## Traceability and Evidence

### Requirement coverage intent

- FR-001/FR-002/FR-007/FR-010: hard production launch gate and deny-safe policy.
- FR-003/FR-011: canonical reason codes and retry semantics.
- FR-004/FR-005: remediation handoff and mandatory re-verification after completion.
- FR-006: explicit environment mode policy and precedence.
- FR-008: cross-domain contracts documented for native/API/web interplay.
- FR-009: freshness and stale-session rejection rules.
- FR-012: auditable decision fields and event references in contracts.

### Success criteria evidence intent

- SC-001/SC-004: production deny proof for invalid/unlinked/suspended/indeterminate states.
- SC-002: blocked-state UX payload clarity and actionability checks.
- SC-003: remediation completion followed by successful next verification when account status is corrected.
- SC-005: transient failure retry evidence showing status update on recovery.

## Complexity Tracking

No constitution violations require exception tracking at planning time.
