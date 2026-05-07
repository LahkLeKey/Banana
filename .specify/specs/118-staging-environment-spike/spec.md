# Feature Specification: Staging Environment Spike (118)

**Feature Branch**: `feature/118-staging-environment-spike`
**Created**: 2026-05-02
**Status**: Spike — research and go/no-go only
**Input**: With `banana.engineer` as production, define a three-tier environment contract (local / preview / production) covering env vars, API URLs, and database isolation.
## Problem Statement

Feature Specification: Staging Environment Spike (118) aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope *(mandatory)*

- Define the three-tier environment model: local, preview (per-PR Vercel URL), production (`banana.engineer`).
- Identify all `VITE_*` vars needed per tier and how they are injected in each context.
- Determine how preview deployments get a staging API URL (not production).
- Determine how database isolation works per tier (`BANANA_PG_CONNECTION`).
- Evaluate Vercel's built-in environment variable scoping (Development / Preview / Production).
- Document a secrets rotation runbook and who owns production credentials.
- Generate a concrete path to specs 129–132.

## Out of Scope *(mandatory)*

- Actual deployment of staging infrastructure (that is spec 130).
- Feature flags platform (separate spec 132).
- React UI or API code changes.

## User Scenarios & Testing *(mandatory)*

### User Story 1 — PR preview uses staging API, not production (Priority: P1)

**Acceptance Scenarios**:

1. **Given** a PR is opened, **When** Vercel deploys a preview, **Then** the preview `VITE_BANANA_API_BASE_URL` points to a staging API, not `api.banana.engineer`.
2. **Given** a merge to main, **When** the production deployment runs, **Then** `VITE_BANANA_API_BASE_URL` points to the production API.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: Each Vercel environment tier (Development, Preview, Production) MUST have its own `VITE_BANANA_API_BASE_URL` value.
- **FR-002**: `BANANA_PG_CONNECTION` MUST use isolated databases per tier.
- **FR-003**: Production secrets MUST NOT be accessible in preview builds.

## Success Criteria *(mandatory)*

### Validation Lane

```bash
python scripts/validate-spec-tasks-parity.py .specify/specs/118-staging-environment-spike
```

## Assumptions

- Vercel's environment variable scoping is sufficient for the three-tier model without a third-party secrets manager.
- A separate staging PostgreSQL instance (e.g., Neon branch or Railway staging) is available.
