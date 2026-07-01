# Steam Auth Purge Inventory and Execution Slices

- Date: 2026-06-30
- Owner: Banana engineering
- Tracking issue: #1213
- Related migration issues: #1209 #1210 #1211 #1212

## Summary

Repository scan (first-party source, tests, docs) found 343 Steam-related matches.

Current distribution:
- API: 208
- React app: 45
- Shared UI package: 46
- Native runtime: 13
- Tests: 20
- Docs: 11

## Highest-impact files

### API
- src/typescript/api/src/routes/game-session.ts (47)
- src/typescript/api/src/routes/auth.ts (35)
- src/typescript/api/src/routes/game-session.test.ts (21)
- src/typescript/api/src/domains/player-identity-account/pipelines/verifyLaunchAttemptPipeline.ts (21)
- src/typescript/api/src/services/authSessionStore.ts (15)
- src/typescript/api/src/routes/auth.test.ts (14)

### React shell
- src/typescript/react/public/notebooks/native-c-catalog.ipynb (11)
- src/typescript/react/src/components/LaunchBlockedPanel.tsx (6)
- src/typescript/react/src/pages/MarketingPage.tsx (5)
- src/typescript/react/src/components/LaunchBlockedPanel.test.tsx (5)
- src/typescript/react/src/pages/SessionRoomPage.tsx (4)
- src/typescript/react/src/pages/LoginPage.tsx (4)

### Shared UI
- src/typescript/shared/ui/src/auth/LoginPage.tsx (14)
- src/typescript/shared/ui/src/auth/session.ts (10)
- src/typescript/shared/ui/src/auth/session.test.ts (10)
- src/typescript/shared/ui/src/auth/SessionRoomPage.tsx (5)

### Native and native tests
- tests/native/runtime/engine/lifecycle/engine_lifecycle_launch_gate_test.c (9)
- tests/native/feedback/native_feedback_loop_factory.c (7)
- src/native/engine/runtime/engine/support/engine_aux_launch_gate.c (6)
- tests/native/runtime/engine/lifecycle/engine_lifecycle_launch_gate_decide_failure_test.c (3)

### Docs
- docs/plans/keycloak-login-implementation-issues.md (3)
- docs/art-archive/steam-placeholders/README-background.txt (3)
- docs/plans/keycloak-login-prd.md (2)
- docs/adr/0003-keycloak-per-environment-fly-apps.md (2)

## Classification

- Runtime auth surface (remove in early slices): API auth routes/session handling, frontend login/session modules, shared UI auth modules.
- Launch-gate policy surface (remove after auth cutover is green): launch gate contracts and native gate behaviors currently phrased around Steam identity.
- Test surface (rewrite after each runtime slice): route tests, integration tests, native lifecycle tests.
- Historical archive surface (do not block runtime migration): docs/art-archive references can stay if clearly marked historical.

## Ordered removal plan

### Slice 1: Frontend and shared UI auth authority switch

Scope:
- Replace Steam start URL and callback parsing with Keycloak/OIDC naming and flow in shared UI auth modules.
- Remove Steam-specific labels and copy in login surfaces.
- Move token handling to in-memory semantics required by PRD.

Files to start:
- src/typescript/shared/ui/src/auth/session.ts
- src/typescript/shared/ui/src/auth/LoginPage.tsx
- src/typescript/react/src/pages/LoginPage.tsx

Exit checks:
- UI smoke task passes.
- Login route triggers Keycloak flow path only.

### Slice 2: API auth route and session shape migration

Scope:
- Remove /auth/steam/start and /auth/steam/callback runtime paths.
- Replace Steam-subject assumptions with authority-neutral or Keycloak subject handling.
- Rename session store identifiers away from steamId in active runtime code.

Files to start:
- src/typescript/api/src/routes/auth.ts
- src/typescript/api/src/services/authSessionStore.ts
- src/typescript/api/src/middleware/requestContext.ts

Exit checks:
- API tests for auth/session pass with Keycloak-only semantics.
- Protected routes reject invalid/missing bearer with expected behavior.

### Slice 3: Game-session and pipeline identity normalization

Scope:
- Remove Steam-auth-required errors and Steam-derived player identity assumptions.
- Normalize player identity derivation to Keycloak subject claim contract.

Files to start:
- src/typescript/api/src/routes/game-session.ts
- src/typescript/api/src/domains/player-identity-account/pipelines/verifyLaunchAttemptPipeline.ts
- src/typescript/api/src/domains/player-identity-account/services/launchGateVerificationService.ts

Exit checks:
- Game-session route tests pass under Keycloak-only identity assumptions.
- Launch attempt pipeline tests updated and passing.

### Slice 4: Native launch-gate policy rename and behavior realignment

Scope:
- Remove Steam-named mode labels and reason codes from active native policy paths.
- Update engine launch-gate contract names to identity-authority-neutral terms.

Files to start:
- src/native/engine/runtime/engine/support/engine_aux_launch_gate.c
- src/native/include/banana_launch_gate_policy.h
- src/native/engine/runtime/engine/lifecycle/engine_lifecycle_launch_gate.c

Exit checks:
- Native focused war suite passes.
- Native launch-gate tests pass with new naming/behavior.

### Slice 5: Test and docs cleanup sweep

Scope:
- Update remaining test fixtures/assertions and integration artifacts.
- Remove Steam-auth references from active runbooks and acceptance docs.
- Preserve art/archive references only where explicitly historical.

Files to start:
- src/typescript/api/src/routes/auth.test.ts
- src/typescript/api/src/routes/game-session.test.ts
- tests/native/runtime/engine/lifecycle/engine_lifecycle_launch_gate_test.c
- docs/plans/keycloak-login-prd.md
- docs/plans/keycloak-login-implementation-issues.md

Exit checks:
- Local auth quickstart works end-to-end without any Steam-auth step.
- No Steam-auth references remain in active auth docs.

## Validation checkpoints

At the end of each slice:
1. Run API smoke and targeted auth tests for touched packages.
2. Run UI smoke and login-path tests for touched packages.
3. For native-touching slices, run native build plus focused war suite.
4. Re-run inventory search and confirm count reduction against this baseline.

## Notes

- This inventory intentionally separates runtime blockers from historical references so delivery is not stalled by archival docs.
- Keycloak-only auth authority is assumed from current PRD and issue updates.
