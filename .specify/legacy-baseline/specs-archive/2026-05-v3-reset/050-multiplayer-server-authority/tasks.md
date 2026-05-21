# Tasks: Spec 050 Multiplayer Server Authority

## Merge Notice (2026-05-15)

- This docket is preserved as completed evidence.
- Active and future execution tasks are consolidated under `.specify/specs/051-ai-gameplay-loop/tasks.md`.

## Constitutional Docket (Dependency Ordered)

This docket executes under Spec 050 jurisdiction. Tasks are ordered by legal dependency.
No later article may execute before its prerequisite tasks are complete.

## Stage 0: Legislative Setup (Blocking)

**Article**: Constitutional preparation and baseline integrity  
**Jurisdiction**: .specify artifacts  
**Agent Authority**: technical writer + orchestration agents

- [x] T001 Confirm active feature pointer remains `.specify/specs/050-multiplayer-server-authority` in `.specify/feature.json`
- [x] T002 Record starting heartbeat checkpoint in `.specify/specs/050-multiplayer-server-authority/heartbeat-log.md`
- [x] T003 Verify plan/spec/tasks constitutional alignment across:
	- `.specify/specs/050-multiplayer-server-authority/spec.md`
	- `.specify/specs/050-multiplayer-server-authority/plan.md`
	- `.specify/specs/050-multiplayer-server-authority/tasks.md`

## Stage 1: Article I - Shared Domain Law (Blocking)

**Article**: I  
**Jurisdiction**: `src/typescript/shared/ui/src/domains`  
**Agent Authority**: shared UI domain agent  
**Prerequisite**: T001-T003

- [x] T010 Define/confirm shared protocol value objects in `src/typescript/shared/ui/src/domains/NetcodeDomain.ts`
- [x] T011 Define/confirm delta and world-state contracts in `src/typescript/shared/ui/src/domains/ReplicationDomain.ts`
- [x] T012 Define/confirm rollback simulator contracts in `src/typescript/shared/ui/src/domains/PredictionDomain.ts`
- [x] T013 Define/confirm transport lifecycle contracts in `src/typescript/shared/ui/src/domains/ConnectionManager.ts`
- [x] T014 Add/refresh unit tests for Article I domains:
	- `src/typescript/shared/ui/src/domains/NetcodeDomain.test.ts`
	- `src/typescript/shared/ui/src/domains/ReplicationDomain.test.ts`
	- `src/typescript/shared/ui/src/domains/PredictionDomain.test.ts`
	- `src/typescript/shared/ui/src/domains/ConnectionManager.test.ts`
- [x] T015 Export Article I domains from `src/typescript/shared/ui/src/index.ts`

## Stage 2: Article II - Application Composition Law

**Article**: II  
**Jurisdiction**: `src/typescript/react/src/pages` and composition roots  
**Agent Authority**: React UI agent  
**Prerequisite**: T010-T015

- [x] T020 Compose multiplayer domain adapters in `src/typescript/react/src/pages/GameEnginePage.tsx` through `GameEngineUIService` boundaries only
- [x] T021 Ensure composition-root constraints and no transport leakage in `src/typescript/react/src/pages/GameEnginePage.tsx`
- [x] T022 Add/refresh React page regression coverage:
	- `src/typescript/react/src/pages/__tests__/GameEnginePage.controller-input.test.tsx`
	- `src/typescript/react/src/pages/__tests__/GameEnginePage.controller-arbitration.test.tsx`
	- `src/typescript/react/src/pages/__tests__/GameEnginePage.context-menu.test.tsx`

## Stage 3: Article III - Server-Orchestration Law

**Article**: III  
**Jurisdiction**: `src/typescript/api/src` and orchestration boundaries  
**Agent Authority**: API pipeline and interop agents  
**Prerequisite**: T010-T015

- [x] T030 Implement/confirm session start contract in `src/typescript/api/src` (`/api/game/session/start`)
- [x] T031 Implement/confirm realtime transport contract in `src/typescript/api/src` (`/api/game/realtime`)
- [x] T032 Implement/confirm session rejoin and end contracts in `src/typescript/api/src` (`/api/game/session/rejoin`, `/api/game/session/end`)
- [x] T033 Add/refresh deterministic sequence and tick invariant tests under `src/typescript/api/src` test surfaces

## Stage 4: Article IV - Native Anti-Cheat Enforcement Law

**Article**: IV  
**Jurisdiction**: native anti-cheat and wrapper/API interop boundaries  
**Agent Authority**: native core + API interop agents  
**Prerequisite**: T030-T033

- [x] T040 Establish API anti-cheat interop boundary corresponding to native anti-cheat exports:
	- `src/native/wrapper/banana_wrapper.c`
	- `src/native/wrapper/banana_wrapper.h`
	- `src/typescript/api/src/lib/anti-cheat-interop.ts` or corresponding API interop boundary
- [x] T041 Enforce usermode telemetry lane scoring in `src/native/core/banana_anticheat.c`
- [x] T042 Enforce suspicious driver lane scoring in `src/native/core/banana_anticheat.c`
- [x] T043 Emit three heartbeat layers (transport/session/simulation-integrity) in server-authoritative loop
- [x] T044 Persist risk score and integrity-hash snapshots in session state for replay/audit evidence
- [x] T045 Add/refresh anti-cheat native tests:
	- `tests/native/test_anticheat.c`
	- `tests/native/test_abi_version.c`

## Stage 5: Article V - Judicial Validation and Evidence Law

**Article**: V  
**Jurisdiction**: unit/integration/evidence surfaces  
**Agent Authority**: test triage + technical writer agents  
**Prerequisite**: T020-T045

- [x] T050 Run shared domain tests for Article I contracts in `src/typescript/shared/ui`
- [x] T051 Run React page regression suite in `src/typescript/react`
- [x] T052 Run native anti-cheat tests in `tests/native`
- [x] T053 Run scoped API orchestration tests for session/transport invariants
- [x] T054 Update completion evidence and legal checkpoints:
	- `.specify/specs/050-multiplayer-server-authority/heartbeat-log.md`
	- `.specify/specs/050-multiplayer-server-authority/plan.md`
	- `.specify/specs/050-multiplayer-server-authority/spec.md`

## Post-Docket Follow-Up (Non-Blocking)

- [ ] F001 Bind TypeScript API anti-cheat adapter to direct native invocation path (FFI or supported bridge) and add parity tests against native score/hash outputs.

# Anti-Cheat Hardening Roadmap (from anti-cheat-drilldown.md)
- [ ] F002 Implement signed challenge/response heartbeats (server_nonce, session_key, keyed MAC)
- [ ] F003 Add time-based plausibility checks (tick-rate, delta validation)
- [ ] F004 Introduce decaying risk model and separate hard/soft risk channels
- [ ] F005 Add server-side behavioral validation (movement, aim, inventory, geometry, reaction-time)
- [ ] F006 Score telemetry entropy/uniformity (variance, jitter, periodicity)
- [ ] F007 Implement session expiration and cleanup (last_activity_ms, stale session removal)
- [ ] F008 Add replay detection and duplicate cache for heartbeats
- [ ] F009 Segment integrity/trust lanes (kernel, usermode, network, gameplay)
- [ ] F010 Ensure thread safety for session store and risk updates
- [ ] F011 Add server-issued simulation challenges (randomized challenge material)
- [ ] F012 Harden integrity hash (keyed, tamper-evident, process-randomized seed)
- [ ] F013 Implement trust levels and response policy (TRUST_NEW, TRUST_ESTABLISHED, etc.)
- [ ] F014 Add audit reconstruction helpers for risk decision replay

## Parallel Execution Rules

- T010-T013 may execute in parallel after T001-T003.
- T014-T015 depend on T010-T013.
- T020-T022 may execute in parallel after T015.
- T030-T033 may execute in parallel after T015.
- T040-T045 may execute in parallel after T030-T033, except T043-T044 depend on T040-T042.
- T050-T053 may execute in parallel after T020-T045.
- T054 executes last after T050-T053.

## DDD and SOLID Compliance Clauses

- Domain entities remain framework-agnostic and pure where practical.
- Value objects remain immutable and serializable.
- Dependencies are injected; constructors/factories do not self-resolve infrastructure.
- Interfaces remain narrow and role-specific (transport, simulation, persistence).
- Presentation layers do not own domain or transport policy.

## Judgment Standard (Done Definition)

- `PredictionDomain`, `ConnectionManager`, `NetcodeDomain`, and `ReplicationDomain` are exported from `@banana/ui`.
- Unit tests for each domain pass in shared UI packages.
- React composes multiplayer domains without boundary violations.
- Anti-cheat heartbeat lanes and policy outputs are observable and test-backed.
- Spec 050 heartbeat/evidence artifacts reflect completed articles and checkpoints.
