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
- `Build / wasm engine assets`
- `Security / WASM penetration tests`
	- Runs `bun run test:wasm-pentest` in `src/typescript/api`
	- Publishes a persistent WASM security artifact bundle under `artifacts/ci/wasm-security`
- `Monorepo / Pass-Fail` (terminal gate)

Add new lanes only when they map to a maintained script/target in this repository.
Prefer extending existing lane scopes before introducing niche policy-only checks.
