# Feature Specification: React Portal 2.5D Game Client Pivot

**Feature Branch**: `001-react-portal-game-client`
**Created**: 2026-05-08
**Status**: Draft
**Input**: Re-align Banana from dashboard/research sprawl into a 2.5D game-client experience delivered through the existing React portal, starting with a WASM-rendered landing surface.

## Problem Statement

The repository accumulated broad, research-heavy specs and workflows that are no longer aligned with the current product goal. The active objective is narrower: keep the existing implementation live while reshaping the React portal into a true game-client runtime shell with a cross-platform, cross-viewport friendly 2.5D rendering approach.

## In Scope

- Preserve current runtime behavior and deployability of the existing React implementation.
- Start over the landing entry surface as a WASM-rendered scene integrated into the React shell.
- Adopt a 2.5D rendering style for the game-client presentation to improve cross-platform and cross-viewport consistency.
- Reflow portal UX toward game-client framing (viewport-first shell, tactical overlays, mission-control language).
- Keep only essential CI/CD workflows required to validate and ship the React portal.
- Remove or archive out-of-scope spec/workflow surfaces from active roots.

## Out of Scope

- New backend platform research programs unrelated to the game-client UX pivot.
- Re-introducing broad autonomous orchestration workflows as active required checks.
- Building a full native 3D engine or platform-specific graphics runtime.
- Backend/data-platform rewrites unrelated to landing and shell rendering direction.

## User Scenarios & Testing

### User Story 1 - WASM Landing Entry (Priority: P1)

As a user, I open the React portal and land in a WASM-rendered 2.5D entry scene that feels like a game client rather than a web page.

**Independent Test**: Open the root route on desktop and mobile-sized viewports and confirm the landing surface is WASM-rendered, responsive, and visually consistent with the game-client direction.

### User Story 2 - Cross-Viewport 2.5D Shell Continuity (Priority: P1)

As a user, I can move from the landing surface into the React shell and retain a consistent 2.5D game-client visual language across key routes and viewport sizes.

**Independent Test**: Validate representative routes at desktop/tablet/mobile widths and confirm layout, overlays, and navigation preserve game-client framing without breaking interaction flows.

### User Story 3 - Safe Delivery Contracts (Priority: P2)

As a maintainer, I can ship the pivot safely with explicit WASM build/runtime contracts and minimal CI validation that keeps scope disciplined.

**Independent Test**: Trigger CI and confirm React build/runtime checks pass with WASM assets and explicit env wiring, without reintroducing workflow/spec sprawl.

### Edge Cases

- Landing fails to initialize when WASM bootstrap artifacts are missing or delayed.
- Render quality degrades on narrow/mobile viewports if 2.5D scene scaling is not bounded.
- Legacy files are reintroduced accidentally by merge/conflict resolution.
- Deploy credentials are unavailable in CI at run time.

## Requirements

### Functional Requirements

- **FR-001**: System MUST render the landing entry route through a WASM-backed renderer integrated into the React app shell.
- **FR-002**: System MUST use a 2.5D rendering direction for the landing and primary shell presentation rather than flat web-page layout conventions.
- **FR-003**: System MUST preserve cross-platform and cross-viewport usability for desktop and mobile-sized viewports.
- **FR-004**: System MUST maintain a working React build path (`bun run build`) with WASM assets included in the build contract.
- **FR-005**: System MUST preserve explicit runtime configuration contracts (including `VITE_BANANA_API_BASE_URL`) required by the portal.
- **FR-006**: System MUST provide CI validation coverage sufficient to detect WASM/bootstrap or rendering-contract regressions.
- **FR-007**: System MUST avoid reintroducing non-pivot workflows/specs into active roots without explicit scope approval.

### Key Entities

- **WasmLandingSurface**: The WASM-rendered landing entry scene presented at the root route.
- **ViewportRenderProfile**: A bounded 2.5D layout and scaling profile for desktop, tablet, and mobile-sized viewports.
- **PortalBuildContract**: Required build inputs, assets, and commands for `src/typescript/react` deployment.
- **ActiveSpecRoot**: The set of currently active specs in `.specify/specs`.

## Success Criteria

### Measurable Outcomes

- **SC-001**: Landing route renders through the WASM surface successfully in CI and local verification flows.
- **SC-002**: 2.5D shell presentation remains usable across defined desktop and mobile-sized viewport profiles.
- **SC-003**: React portal build succeeds in CI with explicit API base URL and WASM asset contracts.
- **SC-004**: Active scope remains constrained to pivot-approved spec/workflow surfaces.

## Assumptions

- Existing React implementation remains the primary runtime delivery surface.
- WASM landing rendering is feasible within current frontend deployment/runtime constraints.
- Vercel remains the deploy target for the React portal.
- Additional specs/workflows can be introduced later only through explicit scope decisions.
