# Feature Specification: API Public Hosting Spike (117)

**Feature Branch**: `feature/117-api-public-hosting-spike`
**Created**: 2026-05-02
**Status**: Spike — research and go/no-go only
**Input**: React app will be deployed to `banana.engineer`. Determine the optimal hosting strategy for the Fastify TypeScript API (`src/typescript/api`) so it is reachable from the production frontend.

## In Scope *(mandatory)*

- Evaluate Vercel Serverless Functions as a host for the Fastify API.
- Evaluate Vercel Edge Functions as an alternative.
- Evaluate a separate PaaS (Railway, Fly.io, Render) as a standalone API host.
- Identify CORS changes needed to accept requests from `https://banana.engineer`.
- Identify the `BANANA_PG_CONNECTION` path in each hosting option.
- Identify which hosting option preserves the Fastify router and Prisma setup without a rewrite.
- Document the recommended option and generate a concrete path to specs 125–128.

## Out of Scope *(mandatory)*

- React deployment (covered by spike 116).
- ASP.NET API hosting (in-scope only if parity is required for banana.engineer).
- Mobile or Electron changes.

## User Scenarios & Testing *(mandatory)*

### User Story 1 — Frontend at banana.engineer calls the API and gets a verdict (Priority: P1)

**Acceptance Scenarios**:

1. **Given** a deployed API, **When** `https://api.banana.engineer/health` is requested, **Then** it returns HTTP 200.
2. **Given** a deployed API with CORS set to `banana.engineer`, **When** the frontend sends a cross-origin request, **Then** it is not blocked by the browser.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The API MUST be accessible from the `banana.engineer` origin.
- **FR-002**: CORS MUST explicitly allow `https://banana.engineer` and `https://www.banana.engineer`.
- **FR-003**: `BANANA_PG_CONNECTION` MUST be injectable as a runtime secret in the chosen host.

## Success Criteria *(mandatory)*

### Validation Lane

```bash
python scripts/validate-spec-tasks-parity.py .specify/specs/117-api-public-hosting-spike
```

## Assumptions

- The Fastify API under `src/typescript/api` is the primary backend for `banana.engineer`.
- A managed PostgreSQL instance (Supabase, Neon, Railway) is available or can be provisioned for production.
