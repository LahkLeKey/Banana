# Plan: Compose-Orchestrated Run Profiles

**Branch**: 041-compose-orchestrated-run-profiles | **Date**: 2026-04-26

## Objective

Make Docker Compose the canonical orchestration path for all supported Banana local run profiles so developer startup, shutdown, and validation behavior is deterministic and reproducible across machines.

## Scope

- Define a profile matrix with explicit compose profile coverage and readiness expectations.
- Align runtime launch entry points to compose-orchestrated flows for all supported profiles.
- Define reproducibility validation for repeated profile runs.
- Document prerequisite checks and troubleshooting guidance for profile startup failures.

## Technical Context

**Language/Version**: Bash shell scripts, Docker Compose, YAML workflow/runtime docs
**Primary Dependencies**: Docker Desktop, Docker Compose, existing Banana orchestration scripts under scripts/
**Storage**: N/A (runtime orchestration contract + docs + script behavior)
**Testing**: Scripted runtime contract checks and existing validation lanes (`validate-ai-contracts`, spec parity checks, profile smoke paths)
**Target Platform**: Windows host + Docker Desktop + Ubuntu WSL2 primary contract; Linux container runtime paths
**Project Type**: Delivery/runtime orchestration and contract documentation
**Performance Goals**: Consistent profile readiness outcomes across repeated runs; deterministic startup ordering and health checks
**Constraints**: Preserve required runtime contracts (`BANANA_PG_CONNECTION`, `BANANA_NATIVE_PATH`, `VITE_BANANA_API_BASE_URL`) and existing compose profile semantics
**Scale/Scope**: All currently supported local run profiles in docker-compose.yml and runtime launch scripts

### Required Runtime Variable Contracts

- `BANANA_PG_CONNECTION`: remains explicit for PostgreSQL-backed runtime and integration paths.
- `BANANA_NATIVE_PATH`: remains explicit for ASP.NET/native wiring paths.
- `VITE_BANANA_API_BASE_URL`: remains explicit for React channel API routing.
- `BANANA_WSL_DISTRO`: remains supported override for Windows -> Ubuntu WSL2 distro selection.

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- **Domain Core First**: PASS. No native domain logic duplication; orchestration-only scope.
- **Layered Interop Contract**: PASS. Preserves existing controller/service/pipeline/native contract variables and runtime wiring.
- **Spec First Delivery**: PASS. Spec and plan artifacts are created before implementation tasks.
- **Verifiable Quality Gates**: PASS. Validation lane is defined in this plan and tied to existing repository checks.
- **Documentation and Wiki Parity**: PASS (required in implementation). Runtime contract/doc updates must ship with orchestration behavior updates.

## Deliverables

- research.md
- data-model.md
- contracts/compose-run-profile-contract.md
- quickstart.md
- tasks.md (generated in next phase)

## Structure

- Runtime orchestration scripts under scripts/
- Compose profile definitions under docker-compose.yml and docker/
- Runtime contract docs under .github/ and README/wiki-linked docs
- Spec artifacts under .specify/specs/041-compose-orchestrated-run-profiles/

## Validation lane

- python scripts/validate-ai-contracts.py
- bash scripts/validate-spec-tasks-parity.sh --all
- compose profile bring-up/down verification using canonical launch paths
- profile readiness verification using service health/status checks per matrix
- deterministic repeat-run checks for selected primary profiles

## Out of scope

- Re-architecting application domain logic
- Introducing new product runtime channels unrelated to current profile matrix
- Replacing every historical helper script in one slice when a compatibility bridge is sufficient
