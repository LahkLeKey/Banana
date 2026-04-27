<!--
Sync Impact Report
- Version change: 1.1.0 -> 1.2.0
- Modified principles:
	- Added VI. One-Window Interactive Validation
	- Added VII. Cross-API Feature Parity
- Added sections: none
- Removed sections: none
- Templates requiring updates:
	- ✅ .specify/templates/plan-template.md (validated, no update required)
	- ✅ .specify/templates/spec-template.md (validated, no update required)
	- ✅ .specify/templates/tasks-template.md (validated, no update required)
- Follow-up TODOs: none
-->
# Banana Spec Kit Constitution

## Core Principles

### I. Domain Core First
All business behavior begins in native C under `src/native`; upper layers orchestrate, validate, and present results, but do not duplicate core domain rules.

### II. Layered Interop Contract
Managed flows must preserve the `controller -> service -> pipeline -> native interop` architecture. Cross-layer behavior changes require explicit contract updates in the touched layers.

### III. Spec First Delivery
AI-assisted work follows Spec Kit stages before implementation via the Specify CLI: `specify specify`, `specify plan`, and `specify tasks`. Emergency fixes may bypass this only with explicit human approval and a follow-up spec update.

### IV. Verifiable Quality Gates
Every change must run domain-appropriate validation before merge, including workflow-required checks for automation pull requests.

### V. Documentation and Wiki Parity
When workflow behavior, automation contracts, prompts, skills, or runbooks change, repository docs and wiki snapshots must be updated in the same SDLC slice.

### VI. One-Window Interactive Validation
Frontend and desktop UX changes must be validated in an interactive in-workspace surface before task closure. For Banana this means using the VS Code integrated browser/web view flow (with compose-backed runtime services) so implementation, diagnostics, and rendered behavior are reviewed in one window.

### VII. Cross-API Feature Parity
When ASP.NET and Fastify expose overlapping capability areas, behavior parity is mandatory. Route availability, status semantics, and response-shape contracts must stay aligned or be explicitly documented as intentional divergence in the active spec before merge.

## Platform Constraints

- Use `BANANA_PG_CONNECTION` whenever PostgreSQL-backed native and integration paths are exercised.
- Keep `BANANA_NATIVE_PATH` explicit for ASP.NET runtime wiring.
- Preserve `VITE_BANANA_API_BASE_URL` as the frontend API base URL contract.
- Preserve feature parity expectations across ASP.NET (`:8080`) and Fastify (`:8081`) for overlapping route contracts used by frontend/runtime channels.
- Preserve Bun as package manager for TypeScript React/Electron/mobile domains.
- Keep runtime-channel behavior aligned with the Windows + Docker Desktop + Ubuntu WSL2 contract.

## Workflow and Review

- Scope changes to the owning domain unless a cross-layer contract actually changes.
- Prefer existing entry points in `scripts/`, workspace tasks, CMake targets, dotnet projects, Bun scripts, and compose profiles.
- Keep automation pull requests on provenance labels including `speckit-driven` and required-check workflows.
- Run `scripts/validate-ai-contracts.py` whenever AI workflow contracts (prompts, agents, skills, instructions, workflows, wiki mapping) are touched.
- Run `scripts/workflow-sync-wiki.sh` in the same change when contract surfaces move.
- For frontend and Electron behavior checks, verify rendered state in the VS Code integrated browser against the active runtime profile before marking tasks done.
- When a change touches overlapping ASP.NET and Fastify API capability, include parity validation evidence (or intentional-drift rationale) in the active Spec Kit artifacts before task closure.

## Governance

This constitution governs Spec Kit driven development and automation workflows in Banana.
Amendments require a pull request that documents rationale, migration impact, and validation updates.
Reviewers should block merges that violate these principles.

**Version**: 1.2.0 | **Ratified**: 2026-04-24 | **Last Amended**: 2026-04-26
