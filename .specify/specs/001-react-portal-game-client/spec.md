# Feature Specification: React Portal Game Client Pivot

**Feature Branch**: `001-react-portal-game-client`
**Created**: 2026-05-08
**Status**: Draft
**Input**: Re-align Banana from dashboard/research sprawl into a slim game-client experience delivered through the existing React portal.

## Problem Statement

The repository accumulated broad, research-heavy specs and workflows that are no longer aligned with the current product goal. The active objective is narrower: keep the existing implementation live while reshaping the React portal into a true game-client runtime shell.

## In Scope

- Preserve current runtime behavior and deployability of the existing React implementation.
- Reflow the React portal UX toward a game-client presentation (viewport-first shell, tactical overlays, mission-control language).
- Keep only essential CI/CD workflows required to validate and ship the React portal.
- Remove or archive out-of-scope spec/workflow surfaces from active roots.

## Out of Scope

- New backend platform research programs unrelated to the game-client UX pivot.
- Re-introducing broad autonomous orchestration workflows as active required checks.
- Rebuilding native engine internals beyond what is needed to support portal UX integration.

## User Scenarios & Testing

### User Story 1 - Launch Into A Game Client (Priority: P1)

As a user, I open the React portal and experience an interface that feels like a game client rather than a document/workbench dashboard.

**Independent Test**: Open primary routes and confirm shell/navigation language, visual hierarchy, and viewport framing match the game-client direction.

### User Story 2 - Ship Safely With Minimal Automation (Priority: P1)

As a maintainer, I can run a slim CI pipeline that validates the React portal build contract without restoring workflow sprawl.

**Independent Test**: Trigger CI and confirm install + build checks run and pass with required env wiring.

### User Story 3 - Keep Scope Disciplined (Priority: P2)

As a team, we can prevent immediate drift back into legacy or random research scope in the active specs/workflows roots.

**Independent Test**: Active roots contain only approved spec/workflow files for the pivot and fail checks when non-allowlisted items are introduced.

### Edge Cases

- Production build fails when `VITE_BANANA_API_BASE_URL` is unset.
- Legacy files are reintroduced accidentally by merge/conflict resolution.
- Deploy credentials are unavailable in CI at run time.

## Requirements

### Functional Requirements

- **FR-001**: System MUST maintain a working React portal build path (`bun run build`) during and after the pivot.
- **FR-002**: System MUST retain only one active pivot spec in `.specify/specs` until new scope is explicitly approved.
- **FR-003**: System MUST provide a minimal React CI workflow in `.github/workflows` that validates install and build.
- **FR-004**: System MUST provide a minimal deployment workflow path for React portal releases.
- **FR-005**: System MUST avoid reintroducing non-pivot workflows/specs into active roots without explicit scope approval.

### Key Entities

- **ActiveSpecRoot**: The set of currently active specs in `.specify/specs`.
- **WorkflowSurface**: The currently active CI/CD workflow files in `.github/workflows`.
- **PortalBuildContract**: Required build inputs and commands for `src/typescript/react` deployment.

## Success Criteria

### Measurable Outcomes

- **SC-001**: `.specify/specs` contains exactly one active feature directory plus root documentation.
- **SC-002**: `.github/workflows` contains only minimal required workflow YAML files for portal CI/deploy.
- **SC-003**: React portal build succeeds in CI with explicit API base URL env wiring.
- **SC-004**: The portal runtime UX aligns with the game-client shell direction across key routes.

## Assumptions

- Existing React implementation remains the primary runtime delivery surface.
- Vercel remains the deploy target for the React portal.
- Additional specs/workflows can be introduced later only through explicit scope decisions.
