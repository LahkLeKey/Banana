# Implementation Plan: Coverage 80 Rehydration for Entire Monorepo

**Branch**: `feature/003-coverage-80-rehydration` | **Date**: 2026-04-25 | **Spec**: `.specify/specs/003-coverage-80-rehydration/spec.md`  
**Input**: Feature specification from `.specify/specs/003-coverage-80-rehydration/spec.md`

## Summary

Establish a deterministic, merge-gated coverage contract that enforces an 80 percent minimum across unit, integration, and e2e layers for all applicable Banana monorepo domains. Implementation will normalize lane outputs from native C, ASP.NET, and TypeScript ecosystems into a single attributable contract, preserve portability and evidence path-safety from 002, and make every failure diagnosable in one run through lane-scoped and aggregate evidence.

## Technical Context

**Language/Version**: GitHub Actions YAML, Bash (GNU/bash on Ubuntu), C/CMake toolchain coverage outputs, .NET 8 test coverage outputs, Bun/TypeScript coverage outputs  
**Primary Dependencies**: existing merge-gated workflows, Docker Compose runtime channels, coverage output normalizers, `actions/upload-artifact`, `actions/download-artifact`, `curl` preflight checks where required  
**Storage**: lane-scoped evidence under repository artifact roots and GitHub Actions artifacts  
**Testing**: merge-gated validation surfaces across unit, integration, and e2e lanes with aggregate contract checks  
**Target Platform**: GitHub-hosted `ubuntu-latest` runners (primary), local Windows + Docker Desktop + Ubuntu WSL2 contract (secondary)  
**Project Type**: CI/test-governance and reporting contract across a multi-language monorepo  
**Performance Goals**: deterministic threshold outcomes; one-pass triage; zero missing coverage evidence for evaluated lanes  
**Constraints**: preserve existing runtime contracts (`BANANA_PG_CONNECTION`, `BANANA_NATIVE_PATH`, `VITE_BANANA_API_BASE_URL`), preserve existing test tooling, avoid machine-specific paths, keep schema parity between docs and validators  
**Scale/Scope**: all merge-gated domain-layer tuples in native, API, web, desktop, and mobile runtime surfaces

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- **I. Domain Core First**: PASS. This feature governs test evidence and CI contracts; it does not alter product domain behavior.
- **II. Layered Interop Contract**: PASS with guardrails. Coverage must validate existing layered boundaries without introducing cross-layer contract drift.
- **III. Spec First Delivery**: PASS. `spec.md` exists and this `plan.md` is created before implementation tasks.
- **IV. Verifiable Quality Gates**: PASS with explicit evidence requirements.
  - Required validation surfaces: unit, integration, and e2e gates for all applicable domain-layer tuples.
  - Required evidence: lane-level normalized coverage result plus aggregate run summary.
- **V. Documentation and Wiki Parity**: PASS. No wiki expansion required unless runtime/operator behavior changes; if behavior changes, parity rules from 002 still apply.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/003-coverage-80-rehydration/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   └── coverage-lane-contract.md
└── tasks.md
```

### Source Code (repository root)

```text
.github/
├── workflows/
│   └── compose-ci.yml
└── copilot-instructions.md

scripts/
├── run-tests-with-coverage.sh
├── run-native-c-tests-with-coverage.sh
├── compose-ci-preflight.sh
├── compose-ci-write-lane-result.sh
└── validate-compose-ci-lane-contract.sh

tests/
src/
```

**Structure Decision**: Keep implementation changes concentrated in delivery/runtime orchestration and coverage-contract surfaces so product feature code remains untouched unless gap-closing tests are required.

## Coverage Contract Alignment

- Reuse 002 lane contract principles: deterministic terminal state, path-safe evidence, preflight dependency classification.
- Enforce one normalized lane result schema for all ecosystems (native/.NET/TypeScript) to avoid format drift.
- Define denominator governance once (exclusions, not-applicable handling, dedup semantics) and require policy snapshots in evidence bundles.
- Keep exception governance strict and time-bounded with explicit ownership and remediation.

## Validation Strategy

- Validate threshold enforcement per layer and per domain tuple.
- Validate failure taxonomy: threshold violation, coverage contract violation, preflight contract violation, flake contract violation.
- Validate aggregate summary completeness and tuple ordering for failed-first triage.
- Validate portability by confirming workspace-relative evidence paths across local and CI runs.
- Validate repeatability through baseline verification windows (10-run stability checks where applicable).

## Phase 0: Research Objectives

- Decide canonical normalization fields across coverage output formats.
- Decide denominator policy semantics and dedup approach for cross-lane overlaps.
- Decide e2e flow manifest contract for runtime-channel denominator tracking.
- Decide exception record lifecycle and expiry handling rules.
- Confirm minimal validation matrix that proves SC-001 through SC-011.

## Phase 1: Design Artifacts

- Produce `research.md` with decisions and alternatives.
- Produce `data-model.md` covering coverage entities, state transitions, and validation rules.
- Produce `contracts/coverage-lane-contract.md` for lane and aggregate payload schema.
- Produce `quickstart.md` with smallest validation sequence for maintainers.
- Re-check constitution gates after design artifacts are complete.

## Phase 2: Implementation Slices (for tasks generation)

- Slice A: Baseline + determinism contract enforcement.
- Slice B: Denominator governance and not-applicable handling.
- Slice C: Unit layer normalization and gate wiring.
- Slice D: Integration layer normalization and preflight classification.
- Slice E: E2e flow manifest denominator and flake-budget behavior.
- Slice F: Aggregate summary and failure taxonomy publication.
- Slice G: Exception governance and expiry enforcement.
- Slice H: Portability and schema parity validation closure.

## Risks and Controls

- **Normalization drift across ecosystems**: control with strict schema contract and parser-failure classification.
- **False confidence from denominator inflation**: control with single exclusion policy and policy snapshots.
- **Flake-induced e2e instability**: control with retry-budget contract and explicit flake failure class.
- **Exception sprawl**: control with required expiry and remediation ownership.

## Complexity Tracking

No constitution violations are expected. If a proposed implementation requires product-layer behavior changes, split those into separate, explicitly scoped follow-on slices with their own validation owner.