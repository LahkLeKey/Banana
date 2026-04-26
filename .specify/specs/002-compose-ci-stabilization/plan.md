# Implementation Plan: Compose CI Stabilization

**Branch**: `feature/002-compose-ci-stabilization` | **Date**: 2026-04-25 | **Spec**: `.specify/specs/002-compose-ci-stabilization/spec.md`
**Input**: Feature specification from `.specify/specs/002-compose-ci-stabilization/spec.md`

## Summary

Stabilize merge-gated Compose CI behavior by making lane terminal states deterministic, publishing lane-scoped diagnostics and fallback artifacts for all outcomes, and removing or explicitly tracking Node runtime deprecation exposure in merge workflows. Delivery focuses on `.github/workflows/compose-ci.yml` and related compose/e2e shell entry points so failures are actionable in a single run.

## Technical Context

**Language/Version**: GitHub Actions YAML, Bash (GNU/bash on Ubuntu), Docker Compose v2, .NET 8, Bun 1.3.9  
**Primary Dependencies**: `actions/checkout`, `actions/setup-dotnet`, `oven-sh/setup-bun`, `actions/upload-artifact`, `actions/download-artifact`, Docker Engine + Compose CLI, `curl`  
**Storage**: Build/test evidence stored in repository `.artifacts/` and GitHub Actions artifacts  
**Testing**: Workflow-driven lane validation plus script-level checks through compose/e2e shell entry points  
**Target Platform**: GitHub-hosted `ubuntu-latest` runners (primary) and local Windows + Docker Desktop + Ubuntu WSL2 contract (secondary)
**Project Type**: CI and runtime orchestration in a multi-language monorepo  
**Performance Goals**: Deterministic lane terminal state for every baseline rerun; one-pass triage with complete diagnostics; zero missing-path upload failures  
**Constraints**: Preserve existing compose profiles and runtime contracts; keep Bun package manager and API/native evidence links intact; avoid product behavior changes  
**Scale/Scope**: One merge-gated workflow plus compose/e2e support scripts and documentation contracts used by maintainers and release owners

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- **I. Domain Core First**: PASS. Scope is workflow/runtime orchestration; no native domain rule changes.
- **II. Layered Interop Contract**: PASS. No controller/service/pipeline/native interop behavior change planned.
- **III. Spec First Delivery**: PASS. `spec.md` exists and this `plan.md` is created before implementation.
- **IV. Verifiable Quality Gates**: PASS with required implementation evidence.
  - Required validation surface: compose lanes, e2e smoke artifacts, native and .NET coverage jobs, and artifact publication outcomes.
- **V. Documentation and Wiki Parity**: PASS with explicit follow-up requirement.
  - Any workflow/runbook behavior changes must be reflected in `.specify/wiki/` and `.wiki/` through the same SDLC slice.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/002-compose-ci-stabilization/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   └── compose-ci-lane-contract.md
└── tasks.md
```

### Source Code (repository root)

```text
.github/
├── workflows/
│   └── compose-ci.yml
└── copilot-instructions.md

docker-compose.yml

scripts/
├── compose-tests.sh
├── compose-runtime.sh
├── compose-electron.sh
├── compose-e2e-bootstrap.sh
├── compose-e2e-teardown.sh
├── run-tests-with-coverage.sh
└── run-native-c-tests-with-coverage.sh

.specify/wiki/
.wiki/
```

**Structure Decision**: Keep all implementation inside the delivery/runtime domain (`.github/workflows`, `scripts`, `docker-compose.yml`) and documentation surfaces (`.specify/wiki`, `.wiki`) to avoid cross-layer drift and preserve existing architecture boundaries.

## Complexity Tracking

No constitution violations are expected for this feature.
