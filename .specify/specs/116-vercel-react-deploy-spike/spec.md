# Feature Specification: Vercel React Deploy Spike (116)

**Feature Branch**: `feature/116-vercel-react-deploy-spike`
**Created**: 2026-05-02
**Status**: Spike — research and go/no-go only
**Input**: Domain `banana.engineer` purchased; Vercel account logged in. Determine the minimum viable config to deploy `src/typescript/react` from this monorepo to Vercel and bind it to `banana.engineer`.

## In Scope *(mandatory)*

- Verify `vercel.json` at repo root correctly targets the React Vite build in `src/typescript/react`.
- Confirm `bun install && bun run build` works as the Vercel build command.
- Confirm `VITE_BANANA_API_BASE_URL` is injectable as a Vercel environment variable.
- Confirm SPA rewrites (`/* → /index.html`) work on Vercel.
- Confirm `banana.engineer` can be bound as the production domain in the Vercel project.
- Confirm Vercel preview deployments generate per-branch URLs automatically.
- Document findings and a concrete path to specs 121–124.

## Out of Scope *(mandatory)*

- API backend hosting (covered by spike 117).
- Staging environment wiring (covered by spike 118).
- Performance/CDN deep-dive (covered by spike 119).
- Observability tooling (covered by spike 120).
- Electron, React Native, or ASP.NET changes.

## User Scenarios & Testing *(mandatory)*

### User Story 1 — Developer opens banana.engineer and sees the Banana UI (Priority: P1)

**Acceptance Scenarios**:

1. **Given** a merge to main, **When** the Vercel deploy completes, **Then** `https://banana.engineer` serves the React app with the correct `<title>`.
2. **Given** a hard-refresh on any sub-path, **When** the SPA rewrite kicks in, **Then** React Router handles the route without a 404 from Vercel.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: `vercel.json` MUST declare `buildCommand`, `outputDirectory`, and `rewrites` for the React SPA.
- **FR-002**: `VITE_BANANA_API_BASE_URL` MUST be documented as a required Vercel environment variable.
- **FR-003**: `banana.engineer` MUST be bindable as the production custom domain in Vercel.

## Success Criteria *(mandatory)*

### Validation Lane

```bash
# vercel.json is valid JSON
python3 -c "import json; json.load(open('vercel.json')); print('OK')"
# React build still exits 0
cd src/typescript/react && bun run build
python scripts/validate-spec-tasks-parity.py .specify/specs/116-vercel-react-deploy-spike
```

## Assumptions

- The Vercel CLI is available (`vercel --version`) or can be installed via `bun add -g vercel`.
- The repo is already linked to a Vercel project or can be linked with `vercel link`.
- `banana.engineer` nameservers have been pointed at Vercel (or Vercel DNS).
