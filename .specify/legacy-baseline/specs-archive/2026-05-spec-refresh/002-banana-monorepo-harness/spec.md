# Feature Specification: Banana Monorepo Workflow Harness

**Feature Branch**: `002-banana-monorepo-harness`
**Created**: 2026-05-09
**Status**: Draft
**Input**: Consolidate all managed CI/CD stages into a single `Banana-Monorepo` harness backed by `.github/workflows/banana.yml`, with domain-driven lane composition and SOLID orchestration rules.

## Problem Statement

Workflow sprawl and inconsistent naming reduce observability and make failure triage harder. Banana needs one canonical workflow harness that presents all managed CI/CD lanes as a single orchestrated graph and terminal summary gate.

## In Scope

- Canonical workflow file path: `.github/workflows/banana.yml`.
- Canonical workflow display name: `Banana-Monorepo`.
- Single-harness orchestration pattern for managed CI/CD lanes.
- Domain-driven lane boundaries and SOLID-style lane composition rules.
- Constitution and workflow docs updates that lock this policy.

## Out of Scope

- GitHub-managed dynamic workflows (for example Copilot internal dynamic workflows).
- Reintroduction of stage-specific standalone workflow files.
- Deep refactors of lane implementation scripts unless required for harness wiring.

## User Scenarios & Testing

### User Story 1 - Single Pane Failure Triage (Priority: P1)

As a maintainer, I can open one monorepo workflow run and understand which domain lane failed and why.

**Independent Test**: Trigger CI on a PR and confirm all managed lanes appear under one `Banana-Monorepo` run with a single terminal pass/fail gate.

### User Story 2 - Stable Orchestration Contract (Priority: P1)

As a workflow author, I can add a new lane without adding a new top-level workflow file.

**Independent Test**: Add a lane job in `banana.yml` with explicit `needs` and verify summary output includes the lane result.

### User Story 3 - Memory-Persistent Governance (Priority: P2)

As a team member, I can rely on Spec Kit constitution rules to prevent workflow sprawl from returning.

**Independent Test**: Constitution and workflow README explicitly reference `Banana-Monorepo` and `.github/workflows/banana.yml` as the canonical harness.

## Requirements

### Functional Requirements

- **FR-001**: The repository MUST use `.github/workflows/banana.yml` as the single managed CI/CD harness file.
- **FR-002**: The harness workflow name MUST be `Banana-Monorepo`.
- **FR-003**: Managed lanes MUST be modeled as domain jobs within the harness and orchestrated with explicit `needs` dependencies.
- **FR-004**: The harness MUST publish a terminal summary/pass-fail stage that reports lane-level outcomes.
- **FR-005**: Governance docs and constitution memory MUST codify the single-harness policy and DDD/SOLID composition rules.
- **FR-006**: Harness lanes that validate domain tooling (for example native procedural generators) MUST enforce DDD/SOLID architecture checks as part of managed CI governance.

### Key Entities

- **WorkflowHarness**: The single orchestrator workflow (`banana.yml`) that coordinates all managed CI/CD lanes.
- **DomainLane**: A bounded workflow job representing one domain responsibility (lint, build, native, runtime, etc.).
- **TerminalGate**: The summary/pass-fail step that consolidates lane outcomes into merge readiness.
- **ArchitectureGateLane**: A managed harness lane that verifies DDD/SOLID architectural boundaries for domain tooling slices.

## Success Criteria

### Measurable Outcomes

- **SC-001**: Only one repository-managed workflow file remains active for managed CI/CD orchestration (`banana.yml`).
- **SC-002**: CI runs present all managed stages under workflow name `Banana-Monorepo`.
- **SC-003**: Terminal gate output includes lane-by-lane result rows and deterministic failure semantics.
- **SC-004**: Constitution and workflow docs include explicit canonical harness name/path and DDD/SOLID rules.
- **SC-005**: Harness design includes an explicit architecture-governance path for enforcing DDD/SOLID constraints in domain tooling changes.

## Assumptions

- Repository-managed CI/CD lanes continue to run through GitHub Actions.
- Dynamic platform-managed workflows outside repository control remain out of scope for this governance slice.
- Domain lanes can be added as jobs within `banana.yml` without introducing new top-level workflow files.
