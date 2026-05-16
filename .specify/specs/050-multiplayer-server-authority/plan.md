# Implementation Plan: Spec 050 Multiplayer Server Authority

## Baseline Context

- Active baseline reset date: 2026-05-15
- Scope retained after stale-spec archive: 050 only
- Current active feature pointer: .specify/specs/050-multiplayer-server-authority

## Constitutional Authority Chain

- Governing constitution: .specify/constitution.md
- Governing spec: .specify/specs/050-multiplayer-server-authority/spec.md
- Governing docket: .specify/specs/050-multiplayer-server-authority/tasks.md
- Enforcement posture: DDD bounded contexts + SOLID responsibility boundaries + server-authoritative multiplayer law

## Objective

Deliver a server-authoritative multiplayer pipeline with native anti-cheat scoring, multi-layer heartbeats, and deterministic reconciliation while preserving the absorbed single-player ARPG contracts now governed by Spec 050.

## Articles of Execution

### Article I: Shared Domain Workstream

**Jurisdiction**: src/typescript/shared/ui/src/domains  
**Agent Authority**: Shared UI domain agent  
**Plan of Action**:

1. Establish protocol value objects as immutable contracts.
2. Harden NetcodeDomain and ReplicationDomain for deterministic reconciliation.
3. Finalize PredictionDomain rollback simulator abstractions.
4. Finalize ConnectionManager lifecycle contracts for transport/session/integrity.

### Article II: Application Composition Workstream

**Jurisdiction**: src/typescript/react/src/pages and composition roots  
**Agent Authority**: React UI agent  
**Plan of Action**:

1. Compose multiplayer domains through GameEngineUIService only.
2. Maintain strict presentation/domain separation in GameEnginePage.
3. Preserve existing UI behavior while introducing authoritative state flow.

### Article III: Server-Orchestration Workstream

**Jurisdiction**: API session and transport orchestration surfaces  
**Agent Authority**: API pipeline and interop agents  
**Plan of Action**:

1. Implement session start/rejoin/end orchestration contracts.
2. Preserve monotonic tick and sequence invariants end-to-end.
3. Maintain replay-audit traceability for authoritative correction flow.

### Article IV: Native Anti-Cheat Workstream

**Jurisdiction**: native anti-cheat scoring and server heartbeat integration  
**Agent Authority**: native core + API interop agents  
**Plan of Action**:

1. Wire API pipeline adapters to native anti-cheat exports.
2. Emit and score usermode telemetry lane signals.
3. Emit and score suspicious ring-1/ring-2 class driver telemetry lane signals.
4. Enforce three heartbeat layers: transport, session, simulation/integrity.
5. Persist risk score and integrity-hash evidence for audit and replay.

### Article V: Judicial Validation and Evidence

**Jurisdiction**: unit/integration suites and spec evidence records  
**Agent Authority**: test triage and technical writer agents  
**Plan of Action**:

1. Validate shared UI and React suites at minimal sufficient scope.
2. Add and run integration suites for reconciliation and heartbeat regressions.
3. Record heartbeat checkpoints and completion evidence in Spec 050 artifacts.

## Compliance Clauses

- Domain entities remain framework-agnostic where practical.
- Value objects remain immutable and serializable.
- Dependencies are injected rather than internally resolved.
- Interfaces remain narrow by responsibility (transport, simulation, persistence).
- Presentation layers do not own domain or transport policy.

## Exit Criteria

- Native anti-cheat tests pass in CI.
- Shared UI and React suites pass for affected domains.
- Multiplayer contract docs, plan, and docket remain synchronized.
- Feature pointer remains on Spec 050 until closure.

## Execution Ledger

- Stage 0 (Legislative Setup): completed
- Stage 1 (Article I Shared Domain): completed
- Stage 2 (Article II Application Composition): completed
- Stage 3 (Article III Server Orchestration): completed
- Stage 4 (Article IV Native Anti-Cheat Enforcement): completed
- Stage 5 (Article V Judicial Validation and Evidence): completed

Validation record:
- Shared UI domain tests (Netcode/Replication/Prediction/ConnectionManager): pass
- React GameEnginePage regression suites: pass
- API game-session route and invariant tests: pass
- Native anti-cheat and ABI tests: pass

Residual follow-up:
- Direct TypeScript API native invocation parity remains a post-docket hardening item; current anti-cheat adapter is the contract boundary used by API routes.
