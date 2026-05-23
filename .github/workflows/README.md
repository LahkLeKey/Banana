# Banana Workflow Harness

This repository uses a single workflow harness:

- `.github/workflows/banana.yml` with workflow name `Banana-Monorepo`

All managed CI/CD lanes must be orchestrated inside this harness so operators can
triage one unified graph and one pass/fail summary.

## Scaffolding Rules (DDD + SOLID)

- Domain first: model each lane as a bounded context (lint, native, runtime, deploy checks).
- Single responsibility: one job owns one domain concern.
- Open/closed: add new domain lanes by adding jobs and wiring `needs`, avoid parallel workflow files.
- Liskov style contracts: keep lane outputs stable so downstream summary logic remains substitutable.
- Interface segregation: keep job inputs minimal and domain-specific env/inputs local to the job.
- Dependency inversion: route heavy logic through versioned scripts under `scripts/` rather than embedding large bash blocks in YAML.

## Merge Gate Contract

- `Banana-Monorepo` is the canonical required check harness.
- Domain lanes must publish clear names and deterministic status.
- `Monorepo / Pass-Fail` remains the terminal decision gate for merge readiness.

## Current Lane Contract

The post-reset `banana.yml` harness keeps only lanes that map to active build entry points:

- `Lint / pre-commit`
- `Build / TypeScript smoke`
	- Includes strict procedural generated-asset contract verification via `src/typescript/react/scripts/prepare-procedural-assets.mjs` with:
		- `BANANA_GENERATED_ASSET_POLICY=strict`
		- `BANANA_GENERATED_ASSET_COMPILE_MODE=skip`
- `Native / build + tests`
- `Native / Neon integration` (runs only when a DB URL secret is present)
- `Monorepo / Pass-Fail` (terminal gate)

## Database Env Contract

- `banana.yml` normalizes database aliases in CI lanes that touch TypeScript
	API/native runtime surfaces.
- Authoritative source order:
	1. `NEON_DATABASE_URL`
	2. `DATABASE_URL`
	3. `BANANA_PG_CONNECTION`
- When a value is present, lane bootstrap syncs all three env vars to the same
	URL before executing lane steps.
- Shared alias logic lives in `scripts/lib/database-env-aliases.sh` and is used
	by CI and deployment/runtime scripts to avoid contract drift.
- `Lint / pre-commit` includes a lightweight helper sanity check to assert
	precedence (`NEON_DATABASE_URL`) and `DATABASE_URL` fallback behavior.
- `Native / Neon integration` executes `scripts/run-native-neon-integration.sh`
	to validate Postgres-enabled native build + strict runtime bootstrap + real SQL probe.

Add new lanes only when they map to a maintained script/target in this repository.
Prefer extending existing lane scopes before introducing niche policy-only checks.
