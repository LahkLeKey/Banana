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

## Architecture-Governance Lane Contract

- `Governance / architecture contracts` is the canonical lane for DDD/SOLID checks on domain tooling slices.
- The lane currently validates native procedural-generation architecture contracts through:
	- `banana_test_engine_asset_generation`
	- `banana_test_engine_asset_compiler_integration`
- Lane result is consumed by `Monorepo / Pass-Fail` summary semantics and participates in terminal merge readiness.
- New architecture-governance checks must be added inside this lane (or jobs wired into its outcome), not via new top-level workflow files.
