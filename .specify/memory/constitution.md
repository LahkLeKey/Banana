<!--
Sync Impact Report
- Version change: 0.0.0-template -> 1.0.0
- Modified principles:
	- Template Principle 1 -> I. Player Trust and Honest Disclosure
	- Template Principle 2 -> II. Open Source and Community Governance
	- Template Principle 3 -> III. Cross-Domain Contract Integrity
	- Template Principle 4 -> IV. Persistent-World Quality Gates
	- Template Principle 5 -> V. Reproducible Multi-Platform Delivery
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

### I. Player Trust and Honest Disclosure
All player-facing claims MUST be verifiable in-repo before release. Store metadata,
AI-generated-content disclosures, online-service disclosures, and minimum system
requirements MUST remain synchronized between implementation, release notes, and
distribution storefronts.
Rationale: persistent online titles lose trust quickly when requirements and runtime
behavior differ from published expectations.

### II. Open Source and Community Governance
Gameplay systems and delivery changes MUST be developed through transparent,
reviewable artifacts: specification, plan, tasks, tests, and changelog evidence.
Changes that impact progression balance, multiplayer fairness, or AI content
generation paths MUST include explicit reviewer-visible rationale.
Rationale: Banana Engineer is an open source MMOARPG and depends on community
confidence in how decisions are made.

### III. Cross-Domain Contract Integrity
Controller -> service -> pipeline -> native interop contracts MUST remain explicit
and testable across `src/native`, `src/typescript/api`, and client runtimes.
Contract changes MUST update the nearest documentation and runtime contracts in the
same change set.
Rationale: the project is a multi-language monorepo; silent drift between layers
causes production regressions and release delays.

### IV. Persistent-World Quality Gates
Every feature MUST define measurable validation for deterministic simulation,
service/API behavior, and user-facing runtime flows. For multiplayer and
persistence-sensitive paths, integration or end-to-end validation is mandatory,
including failure-path assertions.
Rationale: persistent worlds amplify defects; quality gates must catch regressions
before release to players.

### V. Reproducible Multi-Platform Delivery
Official runtime channels MUST be reproducible on the documented Windows + Ubuntu
WSL2 + container workflow, with explicit environment contracts for native assets,
API paths, and client launch commands. Release readiness MUST include evidence for
store requirements (for example controller support declarations and platform notes).
Rationale: reproducibility across local and CI environments is required for stable
launch operations.

## Product and Platform Constraints

- The product scope is a 2.5D persistent-world MMOARPG with real-time combat,
	progression, and community systems.
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

**Version**: 1.0.0 | **Ratified**: 2026-05-21 | **Last Amended**: 2026-05-21
