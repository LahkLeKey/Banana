<!--
Sync Impact Report
- Version change: 1.13.4 -> 2.0.0
- Modified principles:
  - Replaced broad multi-domain governance set with a narrow Spec Kit constitution focused on active specs and execution flow
- Added sections:
  - Active Spec Scope
  - Autopilot Execution Contract
- Removed sections:
  - Infrastructure/deployment-specific constitutional mandates not required for Spec Kit core flow
  - Runtime/engine-specific mandates not required for current planning specs
- Templates requiring updates:
  - ✅ verified compatible .specify/templates/spec-template.md
  - ✅ verified compatible .specify/templates/plan-template.md
  - ✅ verified compatible .specify/templates/tasks-template.md
- Follow-up TODOs:
  - none
-->
# Banana Spec Kit Constitution

## Core Principles

### I. Active Spec Authority
All autonomous work MUST be governed by the active feature pointer in `.specify/feature.json`.
The active spec directory is the only implementation authority for that run; other specs are
lineage or reference unless explicitly absorbed by supersession language.

### II. Spec Kit Sequence Is Mandatory
Feature delivery MUST follow the Spec Kit chain in order: `specify specify`, `specify plan`,
`specify tasks`, then implementation. Skipping a stage requires explicit human approval plus a
recorded rationale in the active spec artifacts.

### III. Confidence and Heartbeat Gate
Autonomous continuation is allowed only when confidence is at least 80%. Each major step MUST
record heartbeat evidence in the active spec folder. If confidence falls below 80%, execution
MUST pause for targeted human clarification before further implementation.

### IV. Bounded Scope and Lane Ownership
Changes MUST stay inside the active spec scope and its declared lane ownership model. Cross-lane
or cross-domain edits are allowed only when the active spec explicitly defines the contract
change and affected paths.

### V. Evidence-Backed Task Closure
Tasks may be marked complete only with evidence in active artifacts (task updates, heartbeat
notes, validator output, or test/build evidence). Claims without repository-traceable evidence
are non-compliant.

### VI. Current Spec Set Focus
Constitutional enforcement is narrowed to current specs under `.specify/specs/`, with priority to
the active feature and directly linked superseding specs. Historical or exploratory governance
rules outside the active spec chain do not block active delivery unless reintroduced by amendment.

### VII. Documentation Parity for Workflow Contracts
When Spec Kit workflow contracts change (instructions, prompts, scripts, validators, templates,
or wiki-mapped workflow docs), updates MUST ship in the same slice to keep operator behavior and
automation behavior aligned.

## Active Spec Scope

- Active feature pointer: `.specify/feature.json`
- Active spec root: `.specify/specs/`
- Current tracked specs in repository scope:
  - `052-v3-pure-c-typescript-reset`
  - `053-mmoarpg-engine-legacy-consumed`

## Platform Constraints

- Preserve the `controller -> service -> pipeline -> native interop` contract for managed flows.
- Use existing repo entry points before creating new orchestration surfaces:
  - `.specify/scripts/bash/*`
  - workspace tasks
  - CMake targets
  - Bun scripts
  - Compose profiles
- Keep runtime environment contracts explicit when applicable:
  - `BANANA_PG_CONNECTION` for PostgreSQL-backed paths
  - `BANANA_NATIVE_PATH` for TypeScript API native wiring
  - `VITE_BANANA_API_BASE_URL` for frontend API routing

## Autopilot Execution Contract

Run this baseline before each autonomous Spec Kit execution pass:

1. `.specify/scripts/bash/spec-extension-preflight.sh --update-first --json`
2. `.specify/scripts/bash/spec-confidence-gate.sh --confidence <n> --step "go-copilot-start" --threshold 80 --notes "startup gate"`
3. Resolve active feature from `.specify/feature.json`
4. Run tasks through active `tasks.md` in dependency order
5. Append heartbeat evidence before/after major execution and validation steps

## Governance

This constitution governs Spec Kit-driven planning and implementation behavior in Banana.
Amendments require a pull request that records rationale, semantic version bump reasoning,
and any template compatibility impacts.

Versioning policy:

- MAJOR: incompatible governance change or large principle set contraction/expansion
- MINOR: new principle or materially expanded workflow obligations
- PATCH: wording clarifications that do not change obligations

Compliance review expectations:

- Reviewers block merges that violate core principles.
- Any approved exception must be recorded in active spec artifacts with owner and expiry.

**Version**: 2.0.0 | **Ratified**: 2026-04-24 | **Last Amended**: 2026-05-18
