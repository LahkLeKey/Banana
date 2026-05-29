# 016 Gameplay/API Continuity Contract Evidence

Date: 2026-05-26

## Scope

Capture API-side continuity payload validation, persistence behavior, and reject-path diagnostics for coherent-world transitions.

## Initial API Contract Inventory

Likely owning API surfaces for this slice:

- `src/typescript/api/src/routes/`
- `src/typescript/api/src/services/`
- `src/typescript/api/src/middleware/`
- `tests/integration/`

Cross-domain continuity bridge expectations:

- Runtime transition payload identifiers and signatures map 1:1 to API persistence schema.
- API validation must reject disconnected/invalid transition payloads with explicit diagnostics.
- Rehydration path must round-trip persisted checkpoints for connected transitions.

## Baseline API Behavior

Discovery snapshot:

- Search under `src/typescript/api/src/**` for `continuity|checkpoint|rehydrat|transition|persist` returned no direct continuity-specific route/service contract yet.
- API smoke import command:
	- `bun -e "import { registerHealthRoutes } from './src/typescript/api/src/routes/health.ts'; console.log(typeof registerHealthRoutes);"`
- Result:
	- `function`

Baseline interpretation:

- API module graph is healthy for route imports.
- Coherent-world continuity persistence contract appears to require new dedicated API route/service surfaces in this feature.

## Planned API Validation Commands

- `bun test tests/integration --filter continuity`
- Focused API smoke command(s) to be finalized after route/service inventory.

## Implemented Continuity Contract Surfaces

- Shared schema ownership:
	- `src/typescript/api/src/lib/contracts/v1/persistentWorld.ts`
		- `ContinuityPayloadSchema`
		- `ContinuityCheckpointCommitRequestSchema`
		- `ContinuityCheckpointCommitViewSchema`
- Domain validation ownership:
	- `src/typescript/api/src/domains/persistent-world-orchestration/services/continuityPayloadService.ts`
		- Canonical route key derivation (`fromVariantId/toVariantId`)
		- Deterministic transition signature derivation
		- Explicit diagnostics for invalid payload, disconnected variant, route key mismatch, and signature drift
- API persistence ingress wiring:
	- `src/typescript/api/src/routes/world.ts`
		- `POST /api/world/continuity/checkpoint`
		- Validates continuity payload through domain service
		- Commits as authoritative delta via replay merge service

## Focused Validation Executions

- Command:
	- `bun test src/domains/persistent-world-orchestration/services/continuityPayloadService.test.ts src/routes/world.continuity.contract.test.ts src/lib/contracts/v1/contracts.test.ts src/routes/world.revisit.contract.test.ts`
- Result:
	- `15 pass, 0 fail`

- Command:
	- `bun run test:continuity`
- Result:
	- `7 pass, 0 fail`

## Foundation Status (T004-T006)

- T004 complete: canonical continuity payload schema and ownership path established.
- T005 complete: route/variant/signature coherence validation helpers active in domain service.
- T006 complete: reject-path diagnostics verified in continuity service and route contract tests.
- T007 complete: native/API continuity harness entry points are both available.

## US1 Write/Rehydrate Continuity Round-Trip (T007-T012)

Implemented API continuity surfaces:

- `POST /api/world/continuity/checkpoint`
	- Canonical continuity payload validation
	- Authoritative continuity checkpoint persistence
- `GET /api/world/continuity/checkpoint/:areaId`
	- Rehydrates latest persisted continuity checkpoint payload + state signature

Harness entry points:

- `bun run test:continuity`
- `bun run test:integration:continuity`

Integration and route evidence commands:

- `bun test src/routes/world.continuity.contract.test.ts src/integration/persistent-world-continuity-roundtrip.integration.test.ts src/domains/persistent-world-orchestration/services/replayMergeService.test.ts`
	- Result: `8 pass, 0 fail`
- `bun run test:continuity`
	- Result: `8 pass, 0 fail`
- `bun run test:integration:continuity`
	- Result: `1 pass, 0 fail`

Status update:

- T007 complete: focused native/API continuity harness entry points available.
- T008 complete: native deterministic continuity round-trip test added and passing.
- T009 complete: API integration continuity round-trip test added and passing.
- T010 complete: continuity checkpoint persistence write path implemented.
- T011 complete: continuity checkpoint rehydration read path implemented.
- T012 complete: runtime/API round-trip evidence captured.

## US2 Contract Alignment Drift + Failure Diagnostics (T013-T016)

Additional drift guard and failure-path coverage:

- Strict continuity schemas (runtime/API drift protection):
	- `src/typescript/api/src/lib/contracts/v1/persistentWorld.ts`
		- `ContinuityCheckpointStateSchema.strict()`
		- `ContinuityPayloadSchema.strict()`
		- `ContinuityCheckpointCommitRequestSchema.strict()`
- Route failure-path diagnostics expanded:
	- `src/typescript/api/src/routes/world.continuity.contract.test.ts`
		- contract version drift (`v2`) rejection
		- unknown payload field rejection
		- missing continuity payload rejection
- Integration drift guard suite:
	- `src/typescript/api/src/integration/persistent-world-continuity-contract-drift.integration.test.ts`

Validation command:

- `bun test src/routes/world.continuity.contract.test.ts src/integration/persistent-world-continuity-contract-drift.integration.test.ts src/domains/persistent-world-orchestration/services/continuityPayloadService.test.ts src/lib/contracts/v1/contracts.test.ts`
	- Result: `19 pass, 0 fail`

US2 status update:

- T013 complete: runtime/API continuity schema drift guards added and passing.
- T014 complete: invalid payload diagnostics failure-path tests added and passing.
- T015 complete: canonical payload validation remains wired at `POST /api/world/continuity/checkpoint` ingress.
- T016 complete: aligned diagnostics evidence captured in API artifact with matching runtime validation suite references.

## US3 Growth Path: New Persisted Field + Unsupported Diagnostics (T017-T020)

New persisted field introduced through owning continuity contract path:

- Field: `continuityPayload.checkpoint.checkpointContextTag`
- Ownership path:
	- `src/typescript/api/src/lib/contracts/v1/persistentWorld.ts`
	- `src/typescript/api/src/domains/persistent-world-orchestration/services/continuityPayloadService.ts`
	- `src/typescript/api/src/routes/world.ts` (`POST /api/world/continuity/checkpoint` and `GET /api/world/continuity/checkpoint/:areaId`)

Mutation safety and diagnostics coverage:

- Mutation safety:
	- `src/typescript/api/src/domains/persistent-world-orchestration/services/continuityPayloadService.test.ts`
		- changing `checkpointContextTag` mutates transition signature deterministically
- Unsupported/disabled field diagnostics:
	- `persistent_world_continuity_checkpoint_context_unsupported`
	- covered in route + integration tests
		- `src/typescript/api/src/routes/world.continuity.contract.test.ts`
		- `src/typescript/api/src/integration/persistent-world-continuity-growth-path.integration.test.ts`

Validation command:

- `bun test src/domains/persistent-world-orchestration/services/continuityPayloadService.test.ts src/routes/world.continuity.contract.test.ts src/integration/persistent-world-continuity-roundtrip.integration.test.ts src/integration/persistent-world-continuity-contract-drift.integration.test.ts src/integration/persistent-world-continuity-growth-path.integration.test.ts src/domains/persistent-world-orchestration/services/replayMergeService.test.ts src/lib/contracts/v1/contracts.test.ts`
	- Result: `28 pass, 0 fail`

Add-a-field workflow (API side):

1. Add the field in `ContinuityCheckpointStateSchema` under `persistentWorld.ts`.
2. Include the field in `expectedTransitionSignature(...)` derivation in `continuityPayloadService.ts`.
3. Add compatibility/unsupported diagnostics in `validateAndNormalize(...)` when required.
4. Extend route + integration tests with both accept and reject paths.
5. Re-run `bun run test:continuity` and `bun run test:integration:continuity` and record results.

## Final Focused API Validation (T023-T024)

Command set and results:

- `bun run test:continuity`
	- Result: `14 pass, 0 fail`
- `bun run test:integration:continuity`
	- Result: `5 pass, 0 fail`
- `bun test src/domains/persistent-world-orchestration/services/replayMergeService.test.ts src/lib/contracts/v1/contracts.test.ts src/routes/world.revisit.contract.test.ts`
	- Result: `11 pass, 0 fail`

Final API summary:

- Canonical continuity payload validation, route ingress diagnostics, round-trip rehydration, strict drift guards, and growth-path field evolution all pass focused closeout validation.
