# Implementation Plan: [FEATURE NAME]

**Branch**: `[###-feature-name]` | **Date**: [DATE] | **Spec**: `.specify/specs/[###-feature-name]/spec.md`
**Input**: Feature specification from `.specify/specs/[###-feature-name]/spec.md`

## Summary

<!-- One paragraph: what this feature does and why it matters. -->

## Technical Context

**Language/Version**: <!-- e.g., TypeScript 5 / Node 22 -->
**Primary Dependencies**: <!-- e.g., Fastify 5, Prisma 7, Bun 1 -->
**Storage**: <!-- e.g., PostgreSQL via Prisma, file artifacts -->
**Testing**: <!-- e.g., Bun test, dotnet test, CTest -->
**Target Platform**: <!-- e.g., GitHub Actions + local Git Bash -->
**Project Type**: <!-- e.g., API, frontend, native, workflow -->
**Performance Goals**: <!-- e.g., Under 10s CI job runtime -->
**Constraints**: <!-- e.g., Must preserve existing safety controls -->
**Scale/Scope**: <!-- e.g., Single-user, monorepo-scoped -->

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- [ ] Existing safety contracts are preserved.
- [ ] No production deployment bypasses are introduced.
- [ ] All changes are auditable by persisted artifacts and PR evidence.

## Project Structure

<!-- List the primary files/directories this spec touches -->
```
.specify/specs/[###-feature-name]/
scripts/
src/
tests/
```

## Phases

### Phase 1: Setup

**Goal**: Establish scaffolding and verify prerequisites.

| Task | File(s) | Acceptance |
|------|---------|------------|
| <!-- Task 1 --> | <!-- paths --> | <!-- acceptance --> |

### Phase 2: Implementation

**Goal**: Deliver the core functionality.

| Task | File(s) | Acceptance |
|------|---------|------------|
| <!-- Task 1 --> | <!-- paths --> | <!-- acceptance --> |

### Phase 3: Validation

**Goal**: Confirm all success criteria are met.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Spec quality gate passes | `scripts/validate-spec-quality.sh` | Exit 0 |
| CI smoke passes | `.github/workflows/` | All checks green |
