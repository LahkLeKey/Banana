# Implementation Plan: API MMO Recenter

**Branch**: `feature/steam-login-persistent-world` | **Date**: 2026-05-24 | **Spec**: `.specify/specs/007-api-mmo-recenter/spec.md`

**Input**: Feature specification from `.specify/specs/007-api-mmo-recenter/spec.md`

## Summary

Re-center the TypeScript API as the MMO orchestration backbone for DX12 client flows while exposing separately governed website-facing player APIs that preserve one authoritative player truth model. The design uses four explicit bounded subdomains, versioned contract namespaces, Neon serverless Postgres as the sole system-of-record, and Fly.io-ready runtime/deploy gates with auditable change history and failure-path validation.

## Technical Context

**Language/Version**: TypeScript 5.x on Bun runtime

**Primary Dependencies**: Fastify 5, @fastify/helmet, @fastify/rate-limit, jose, ws, zod, pg-boss

**Storage**: Neon serverless Postgres (authoritative persistent store via runtime alias sync)

**Testing**: `bun test`, API parity governance scripts, native-Neon integration lane (`bun run test:integration:native-neon`), cross-channel integration suites to be added in implementation

**Target Platform**: Fly.io-hosted API service for staging/production; DX12 Windows client and website consumers

**Project Type**: TypeScript Fastify web service in monorepo with native/runtime integrations

**Performance Goals**: Maintain sub-2s consistency reflection for accepted concurrent gameplay mutations (SC-001), protect gameplay orchestration from website-read load contention

**Constraints**: Preserve controller -> service -> pipeline -> native interop direction; no drift between gameplay/web truth models; exactly-once mutation settlement semantics; backward-compatible contract evolution

**Scale/Scope**: API bounded-context recentering across gameplay session orchestration, player identity/account, progression/inventory, insights read model, Fly deployment readiness, and Neon persistence consistency

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- [x] Player trust and disclosure alignment verified. Scope is API/runtime reliability and channel consistency; no immediate storefront copy or disclosure claim change is introduced.
- [x] Storefront governance artifacts identified when public Steam copy is affected. Not required for this scope unless release messaging adds new public claims; if it does, storefront contract workflow is mandatory.
- [x] Cross-domain contracts mapped for touched layers (native/API/client/runtime). Planned contracts cover API bounded contexts, DX12/gameplay route surface, website player route surface, and Fly/Neon runtime constraints.
- [x] Quality gates defined with measurable checks for deterministic behavior, integration paths, and failure handling. Gates include cross-channel consistency, idempotency/duplicate-prevention, and deployment readiness.
- [x] Reproducible delivery path identified for target runtime channels and evidence artifacts listed for release validation. Validation routes through Bun/API scripts, Neon integration lane, and Fly deployment checks.

**Post-Design Re-check**: Pass. Research, data model, and contracts resolve pre-plan uncertainties with no unresolved constitution conflicts.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/007-api-mmo-recenter/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   ├── bounded-context-ownership.md
│   ├── api-surface-contract.md
│   └── fly-neon-runtime-contract.md
└── tasks.md
```

### Source Code (repository root)

```text
src/typescript/api/
├── fly.toml
├── src/
│   ├── index.ts
│   ├── routes/
│   │   ├── auth.ts
│   │   ├── game-session.ts
│   │   ├── health.ts
│   │   └── world.ts
│   ├── domains/
│   │   └── chat/
│   ├── middleware/
│   ├── plugins/
│   ├── services/
│   │   ├── databaseRuntime.ts
│   │   ├── nativePgBouncer.ts
│   │   └── authSessionStore.ts
│   └── lib/
└── README.md

scripts/
└── deploy-api-fly.sh
```

**Structure Decision**: Keep implementation centered in `src/typescript/api` with new bounded subdomain folders and route/service contracts added in-place. No cross-domain code movement is required at planning time.

## Storefront Deliverables

Not applicable for this feature as currently scoped. No direct public storefront copy or asset changes are introduced by planning artifacts.

## Phase Plan

### Phase 0: Research (complete)

- Resolved subdomain ownership model, versioned API namespace policy, idempotency strategy, Neon source-of-record contract, Fly readiness gates, and read/write isolation approach.
- Output: `research.md`

### Phase 1: Design and Contracts (complete)

- Captured persistent entities and state transitions in `data-model.md`.
- Defined domain ownership and API/runtime contracts under `contracts/`.
- Created execution and validation guidance in `quickstart.md`.

### Phase 2: Task Planning Inputs (prepared for `/speckit.tasks`)

Implementation work should be decomposed into mergeable slices:

| Slice | Goal | Primary Areas | Validation Focus |
|-------|------|---------------|------------------|
| S1 | Create bounded subdomain scaffolding and ownership map in API code | `src/typescript/api/src/domains`, route registration boundaries | Build + unit smoke + parity inventory |
| S2 | Introduce `/v1/gameplay/*` authoritative orchestration endpoints | gameplay routes/services/pipeline wiring | Multi-client orchestration integration tests |
| S3 | Introduce `/v1/player/*` account/progression/inventory/insight endpoints | player routes/services/read models | Cross-channel consistency and no-data semantics tests |
| S4 | Enforce exactly-once ledger settlement and conflict resolution | persistence service layer + database schema/migrations as needed | Failure injection + duplicate prevention assertions |
| S5 | Harden Fly/Neon runtime readiness and rollback evidence | `fly.toml`, deploy/runtime scripts/docs | Health/connectivity/deploy rollback checks |

## Validation Strategy

1. Baseline: `cd src/typescript/api && bun run test && bun run parity:inventory`.
2. Strict contract drift check before closeout: `bun run parity:validate`.
3. Persistence integration: `NEON_DATABASE_URL=... bun run test:integration:native-neon`.
4. Feature-specific integration suites (to be implemented in tasks) covering:
   - Concurrent gameplay session state consistency.
   - DX12/web shared account/progression/inventory truth parity.
   - Insights derivation correctness and no-data behavior.
   - Duplicate-grant prevention under concurrent mutation failure injection.
5. Deployment readiness: Fly deploy helper execution with recorded health/startup/connectivity/rollback evidence.

## Traceability And Evidence

### Requirement coverage intent

- FR-001/FR-006: bounded-context ownership contract + route ownership enforcement.
- FR-002: gameplay orchestration endpoint contract and validation slices.
- FR-003/FR-005: shared account/progression/inventory truth via `/v1/player/*` contracts.
- FR-004: idempotency and deterministic conflict-resolution settlement model.
- FR-007: Neon source-of-record runtime contract.
- FR-008: Fly runtime/deploy readiness gates.
- FR-009: domain change/audit record model.
- FR-010/FR-011/FR-012: versioned compatibility + explicit error semantics + authorization boundaries.

### Success criteria evidence intent

- SC-001: multi-client orchestration consistency reports.
- SC-002: cross-channel record parity validation outputs.
- SC-003: website insight usability checks and response semantics.
- SC-004: failure-injection duplicate-prevention evidence.
- SC-005: Fly readiness artifact bundle.
- SC-006: bounded-context ownership matrix with no overlap exceptions.

## Complexity Tracking

No constitution violations require exception tracking at planning time.
