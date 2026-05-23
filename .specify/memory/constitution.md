<!--
Sync Impact Report
- Version change: 1.0.0 -> 1.1.0
- Modified principles:
	- Template Principle 1 -> I. Native UI ABI Wrapper and Marshaling
	- Template Principle 2 -> II. Player Trust and Honest Disclosure
	- Template Principle 3 -> III. Open Source and Community Governance
	- Template Principle 4 -> IV. Cross-Domain Contract Integrity
	- Template Principle 5 -> V. Persistent-World Quality Gates
	- Template Principle 6 -> VI. Reproducible Multi-Platform Delivery
- Added sections:
	- Product and Platform Constraints
	- Development Workflow and Release Quality Gates
- Removed sections:
	- None
- Templates requiring updates:
	- ✅ .specify/templates/plan-template.md
	- ✅ .specify/templates/spec-template.md
	- ✅ .specify/templates/tasks-template.md
	- ⚠ pending .specify/templates/commands/*.md (directory not present in this repo layout)
- Follow-up TODOs:
	- None
-->

# Banana Engineer Constitution

## Core Principles

### I. Native UI ABI Wrapper and Marshaling
All UI-specific behavior MUST pass through a native UI ABI wrapper that marshals
engine state, input, and presentation data into each runtime-specific viewport.
React, Electron, web, and mobile shells are thin hosts only; they MUST NOT own
UI-specific business logic or gameplay decisions. Any UI-facing contract change
MUST update the wrapper, shared UI contract, and consuming runtimes in the same
change set.
Rationale: the game needs one native-backed UI contract so presentation layers can
be swapped or reused without reintroducing UI-owned gameplay logic or duplicated
platform behavior.

### II. Player Trust and Honest Disclosure
All player-facing claims MUST be verifiable in-repo before release. Store metadata,
AI-generated-content disclosures, online-service disclosures, and minimum system
requirements MUST remain synchronized between implementation, release notes, and
distribution storefronts.
Rationale: persistent online titles lose trust quickly when requirements and runtime
behavior differ from published expectations.

### III. Open Source and Community Governance
Gameplay systems and delivery changes MUST be developed through transparent,
reviewable artifacts: specification, plan, tasks, tests, and changelog evidence.
Changes that impact progression balance, multiplayer fairness, or AI content
generation paths MUST include explicit reviewer-visible rationale.
Rationale: Banana Engineer is an open source MMOARPG and depends on community
confidence in how decisions are made.

### IV. Cross-Domain Contract Integrity
Controller -> service -> pipeline -> native interop contracts MUST remain explicit
and testable across `src/native`, `src/typescript/api`, and client runtimes.
Contract changes MUST update the nearest documentation and runtime contracts in the
same change set.
Rationale: the project is a multi-language monorepo; silent drift between layers
causes production regressions and release delays.

### V. Persistent-World Quality Gates
Every feature MUST define measurable validation for deterministic simulation,
service/API behavior, and user-facing runtime flows. For multiplayer and
persistence-sensitive paths, integration or end-to-end validation is mandatory,
including failure-path assertions.
Rationale: persistent worlds amplify defects; quality gates must catch regressions
before release to players.

### VI. Reproducible Multi-Platform Delivery
Official runtime channels MUST be reproducible on the documented Windows + Ubuntu
WSL2 + container workflow, with explicit environment contracts for native assets,
API paths, and client launch commands. Release readiness MUST include evidence for
store requirements (for example controller support declarations and platform notes).
Rationale: reproducibility across local and CI environments is required for stable
launch operations.

Repository-local shell automation MUST be Bash-first and Bash-authored. Checked-in
PowerShell scripts are prohibited; if a workflow needs a script in-repo, it MUST be
implemented as a `bash` script and invoked through the documented Bash entry point.
PowerShell may appear only in external operator instructions when a third-party tool
requires it and no repository script is introduced.

## Product and Platform Constraints

- The product scope is a 2.5D persistent-world MMOARPG with real-time combat,
	progression, and community systems.
- UI shells MUST remain thin viewports over the native UI ABI wrapper; game-facing
	runtime behavior belongs in native or shared contract layers, not in app-local UI
	business logic.
- Minimum supported release platform is Windows 10 64-bit or newer.
- Online and interactive elements MUST disclose third-party integrations used for
	AI content generation in release materials.
- Community feedback and open source contribution flow MUST remain first-class input
	to roadmap and balancing decisions.

## Development Workflow and Release Quality Gates

- Spec-driven execution is mandatory: `/speckit.specify` -> `/speckit.plan` ->
	`/speckit.tasks` -> implementation.
- Each plan MUST pass a constitution check before design and before implementation.
- Each release-affecting change MUST include scoped tests and evidence artifacts for
	touched domains (native, API, UI/runtime, or orchestration).
- Pull requests MUST summarize principle compliance and any explicit exceptions.
- Repository-owned automation MUST use Bash (`#!/usr/bin/env bash`) as the default
	shell. Checked-in `.ps1` scripts are not allowed in this repository.

## Governance

This constitution supersedes conflicting local process notes.

Amendment process:
- Propose amendments through a pull request that includes motivation, impact scope,
	migration or remediation tasks, and template synchronization updates.
- Approval requires maintainers from affected domains (native, API/runtime,
	frontend/Electron/mobile) when cross-domain behavior changes.

Versioning policy:
- MAJOR: removes or redefines a principle in a backward-incompatible way.
- MINOR: adds a principle or materially expands normative requirements.
- PATCH: clarifies wording without changing governance intent.

Compliance review expectations:
- Plan and PR reviews MUST explicitly confirm constitution alignment.
- Exceptions MUST be time-bound, documented, and linked to follow-up tasks.
- Release readiness checks MUST include evidence that storefront disclosures and
	runtime behavior remain aligned.

**Version**: 1.2.0 | **Ratified**: 2026-05-21 | **Last Amended**: 2026-05-22
