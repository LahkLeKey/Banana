# Quickstart: API MMO Recenter

## Purpose

Use this guide to validate planning assumptions and prepare implementation slices for the MMO-oriented API recenter effort.

## Prerequisites

- Run from repository root.
- Bun installed and available.
- Neon connection string available for integration/deployment checks.
- Fly CLI authenticated for deployment-readiness checks.

## Baseline API Validation

```bash
cd src/typescript/api
bun run test
bun run parity:inventory
```

Optional strict parity gate (recommended before closing implementation slices):

```bash
cd src/typescript/api
bun run parity:validate
```

## Neon Integration Validation

```bash
cd src/typescript/api
NEON_DATABASE_URL="postgres://..." bun run test:integration:native-neon
```

Expected result: API runtime initializes with one authoritative Neon URL alias set and a successful SQL probe lane.

## Local Runtime Smoke

```bash
cd src/typescript/api
BANANA_ENGINE_ADAPTER=inmemory HOST=0.0.0.0 PORT=8081 bun run dev
```

Validate:
- `GET /health` returns healthy status.
- Existing auth/session routes remain reachable.
- New MMO/domain routes (added during implementation) register under versioned namespaces without breaking existing contracts.

## Fly.io Readiness and Deploy Contract

From repo root:

```bash
NEON_DATABASE_URL="postgres://..." bash scripts/deploy-api-fly.sh
```

Readiness evidence to capture:
- Successful startup in Fly environment.
- `/health` checks passing.
- No unresolved Neon connectivity blockers.
- Rollback command/path documented in deployment notes.

## Cross-Channel Validation Targets

Implementation slices should produce evidence for:
1. DX12 gameplay session orchestration consistency under concurrent players.
2. Shared account/progression/inventory truth across DX12 and website APIs.
3. Website insights no-data semantics and freshness metadata behavior.
4. Duplicate-grant prevention under injected concurrent mutation scenarios.

## Planned Evidence Paths

Store validation outputs in feature-local or artifacts paths, for example:

- `.specify/specs/007-api-mmo-recenter/checklists/`
- `artifacts/api/007-us1-session-orchestration.txt`
- `artifacts/api/007-us2-cross-channel-consistency.txt`
- `artifacts/api/007-us3-player-insights.txt`
- `artifacts/api/007-failure-injection-idempotency.txt`
- `artifacts/api/007-fly-readiness.txt`
