# Feature Specification: Speckit Bootstrapping Improvement

**Feature Branch**: `104-speckit-bootstrapping-improvement`
**Created**: 2026-05-02
**Status**: Stub
**Wave**: automation
**Domain**: workflow / orchestration / dx
**Depends on**: #103

## Problem Statement

As spec drain (#103) runs autonomously and processes more specs, the quality of each spec's bootstrapping artifacts (spec.md, plan.md, tasks.md) directly determines how many specs require human correction before they can be executed. Today's bootstrapping flow has several friction points: spec stubs are generic placeholders requiring manual fill-in, plans lack domain-aware context, tasks are sometimes too coarse or missing validation/evidence hooks, and `create-new-feature.sh` produces minimal scaffolding that cannot be passed directly to a planner or implementer agent.

## In Scope *(mandatory)*

- Enrich `create-new-feature.sh` to accept `--description "<text>"` that pre-populates Problem Statement, In Scope, and Out of Scope sections.
- Add a keyword-driven domain classifier (native, api, frontend, infra, workflow, data, security, dx) to pre-select wave tag and planner hints.
- Generate a skeleton `plan.md` at creation time with pre-filled Technical Context derived from the domain classifier.
- Add `scripts/validate-spec-quality.sh` that checks for unfilled placeholders, missing sections, and absent Success Criteria.
- Integrate the quality gate into spec drain (#103) so low-quality specs are flagged `BLOCKED: quality_gate` rather than failing mid-implementation.
- Add a `speckit:bootstrap` target to `scripts/workflow-orchestrate-sdlc.sh` that runs `specify specify`, `specify plan`, and `specify tasks` in sequence.

## Out of Scope *(mandatory)*

- Rewriting or replacing the Specify CLI itself.
- Automatically generating implementation code from spec content.
- Multi-spec batch bootstrapping (one spec at a time is sufficient for v1).
- LLM-powered spec enrichment beyond keyword-driven domain classification.

## User Scenarios

### User Story 1 — Pre-populated Spec Stub (Priority: P1)

An operator runs `create-new-feature.sh` with `--description` and gets a spec.md with no unfilled placeholder text.

**Independent Test**: Run script with `--description "add rate limiting to the API"` and assert no `[ACTION REQUIRED]` strings in generated spec.md.

### User Story 2 — Domain-Aware Plan Skeleton (Priority: P1)

An operator bootstraps an infra spec and the generated `plan.md` already has the correct tech context (Bash, Docker Compose, GitHub Actions).

**Independent Test**: Create spec with "docker compose" in description; verify generated plan.md has `Language/Version: Bash` pre-filled.

### User Story 3 — Spec Quality Gate (Priority: P2)

Before spec drain implements a spec, a quality gate blocks incomplete specs with `BLOCKED: quality_gate` instead of failing mid-implementation.

**Independent Test**: Run `scripts/validate-spec-quality.sh` against a placeholder-filled spec and assert exit code 1 with identifying message.

### User Story 4 — One-Command Spec Bootstrapping (Priority: P2)

`speckit:bootstrap <spec-number>` runs the full three-step Specify CLI flow and produces all three artifacts.

**Independent Test**: Run `bash scripts/workflow-orchestrate-sdlc.sh speckit:bootstrap 104` and verify spec.md, plan.md, tasks.md all exist and quality gate passes.

## Functional Requirements

- FR-1: `create-new-feature.sh` MUST accept `--description "<text>"` and pre-populate spec.md sections.
- FR-2: Domain classification MUST cover at least 6 domain buckets.
- FR-3: Generated `plan.md` skeleton MUST include pre-filled Technical Context.
- FR-4: `validate-spec-quality.sh` MUST exit 1 with human-readable output for placeholder-filled specs.
- FR-5: `validate-spec-quality.sh` MUST exit 0 for well-formed specs.
- FR-6: Spec drain integration MUST block specs failing the quality gate with reason code `quality_gate`.

## Success Criteria

- `create-new-feature.sh --description` produces spec.md with no unfilled action-required placeholders.
- Quality gate detects at least 5 placeholder pattern types (action-required, placeholder, in-scope-outcome-N, brief-title, DATE) and emits readable errors.
- Spec drain classifies low-quality specs as BLOCKED rather than failing mid-implementation.
- `speckit:bootstrap` command successfully produces all three Specify CLI artifacts.

## Notes for the planner

- Keep domain classifier as a simple keyword array with no external dependencies.
- `validate-spec-quality.sh` should be compatible with the `--spec` flag pattern from `validate-spec-boundaries.sh`.
- Embed `plan.md` skeleton template in a companion file under `.specify/templates/`.
