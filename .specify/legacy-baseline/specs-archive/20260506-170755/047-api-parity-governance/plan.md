# Implementation Plan: API Parity Governance

**Branch**: `041-compose-orchestrated-run-profiles` | **Date**: 2026-04-26 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `.specify/specs/047-api-parity-governance/spec.md`

## Summary

Define and enforce a parity governance contract for overlapping ASP.NET (`:8080`) and Fastify (`:8081`) API capability areas so frontend/runtime consumers receive stable route availability, status semantics, and response shapes. The plan introduces a parity inventory artifact, explicit parity exception governance, and gate logic that fails delivery when unresolved overlap drift is detected.

## Technical Context

**Language/Version**: Markdown planning artifacts, C# (ASP.NET API), TypeScript (Fastify API), YAML/shell workflow surfaces
**Primary Dependencies**: ASP.NET controllers and pipeline surfaces, Fastify route modules, existing compose/runtime contract, Spec Kit artifacts
**Storage**: File-based governance artifacts in `.specify/specs/047-api-parity-governance` (no new runtime datastore)
**Testing**: API parity validation workflow checks, route inventory verification, drift-detection evidence captured in quickstart
**Target Platform**: Local and CI delivery pipelines for Banana monorepo (Windows + Docker Desktop + Ubuntu WSL2 runtime contract)
**Project Type**: Cross-API governance and validation planning slice
**Performance Goals**: Detect and report parity drift during normal validation windows before merge; no additional runtime latency requirement introduced by planning artifacts
**Constraints**: Must preserve Layered Interop architecture, must satisfy Constitution VII (Cross-API Feature Parity), must keep intentional divergence explicitly documented and time-bounded
**Scale/Scope**: Overlapping capability areas between ASP.NET controllers and Fastify routes; parity inventory, drift findings, exceptions, and workflow gates

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

### Gate Evaluation Logic

- Pass rule: every constitution principle must be marked PASS with implementation evidence in this plan's artifacts.
- Fail rule: any principle marked FAIL or any unresolved `NEEDS CLARIFICATION` item blocks planning completion.
- Special parity rule (Principle VII): overlapping capability areas across ASP.NET and Fastify must have parity evidence or an approved time-bounded exception.

### Pre-Design Gate Check

- Gate I (Domain Core First): PASS. No native business-rule duplication introduced; this is governance and parity validation planning.
- Gate II (Layered Interop Contract): PASS. No controller/service/pipeline/native-interop contract mutation in planning scope.
- Gate III (Spec First Delivery): PASS. Active spec exists for feature 047 and planning artifacts are generated from it.
- Gate IV (Verifiable Quality Gates): PASS. Plan requires parity drift checks with explicit fail behavior.
- Gate V (Documentation and Wiki Parity): PASS. Governance behavior is captured in feature documentation and contract artifacts.
- Gate VI (One-Window Interactive Validation): PASS. Quickstart includes integrated runtime checks and rendered frontend stability validation.
- Gate VII (Cross-API Feature Parity): PASS. Plan defines overlapping-route inventory, parity findings taxonomy, and exception governance.

### Post-Design Gate Check

- Gate I: PASS after data model and contracts confirm no domain-rule relocation.
- Gate II: PASS after design preserves current API layering boundaries.
- Gate III: PASS after Phase 0/1 artifacts completed for this feature.
- Gate IV: PASS after quickstart captures enforceable parity validation sequence.
- Gate V: PASS after parity contract artifact creation under `contracts/`.
- Gate VI: PASS after integrated validation and runtime evidence path documented.
- Gate VII: PASS after parity contract defines route/status/shape parity and exception controls.

No unresolved gate failures remain. Planning output is invalid if any gate flips to FAIL during implementation.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/047-api-parity-governance/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
└── tasks.md
```

### Source Code (repository root)

```text
src/c-sharp/asp.net/
├── Controllers/
├── Pipeline/
└── NativeInterop/

src/typescript/api/src/
├── domains/*/routes.ts
└── routes/*.ts

scripts/
└── (validation and CI entry points reused)

.github/workflows/
└── (parity enforcement integration target)
```

**Structure Decision**: Keep implementation and validation in existing ASP.NET and Fastify route surfaces while storing governance artifacts in this feature spec folder. Reuse current workflow/script entry points instead of adding parallel orchestration.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| None | N/A | N/A |
