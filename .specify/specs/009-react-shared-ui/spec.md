# Feature Specification: React App + Shared UI (v2)

**Feature Branch**: `009-react-shared-ui`  
**Created**: 2026-04-25  
**Status**: Draft  
**Parent**: [`005-v2-regeneration`](../005-v2-regeneration/spec.md)  
**Baseline**: [`legacy-baseline/react-shared-ui.md`](../../legacy-baseline/react-shared-ui.md)

## Summary

Regenerate the React app (`src/typescript/react`) and shared UI package
(`src/typescript/shared/ui`) with a documented public component API,
deduplicated build configs, and a clear stance on what RN can/cannot reuse.

## User Scenarios & Testing

### User Story 1 — Documented shared UI public surface (Priority: P1)

As a consumer (web app or Electron renderer), I import shared components from
a documented public API; deep imports are forbidden.

### User Story 2 — Single source for Tailwind / Vite config (Priority: P1)

As a developer, there is one canonical `tailwind.config.ts` and one canonical
`vite.config.ts`; `.js` and `.d.ts` siblings are absent or generated.

### User Story 3 — Stable API contract (Priority: P1)

As the frontend, I read `VITE_BANANA_API_BASE_URL` and call routes from
`007/008` per the ownership contract; no hardcoded base URLs.

### User Story 4 — RN reuse boundary (Priority: P2)

As an RN developer (`011`), I know which shared primitives are
platform-agnostic and which are web-only (Tailwind-bound).

### Edge Cases

- A shared component pulls in a web-only dep used by RN by accident → caught by lint or import boundary check.
- `.dockerignore` lets host `node_modules` leak into the container build → forbidden, see `012`.

## Requirements

### Functional Requirements

- **FR-001**: v2 MUST expose a documented public API in `@banana/ui`; deep imports are linted out.
- **FR-002**: v2 MUST keep one canonical `tailwind.config.ts` and one canonical `vite.config.ts`.
- **FR-003**: v2 MUST keep `VITE_BANANA_API_BASE_URL` as the API base URL contract.
- **FR-004**: v2 MUST keep Bun as the package manager.
- **FR-005**: v2 MUST mark which shared components are web-only vs cross-platform.
- **FR-006**: Shared UI package name `@banana/ui` MUST be preserved.

### Hard contracts to preserve

- `VITE_BANANA_API_BASE_URL`.
- `@banana/ui` package name.
- Bun PM.
- `.dockerignore` excludes `**/node_modules` so host symlinks do not overwrite Linux installs.
- `docker/react.Dockerfile` shared-UI install order: copy shared package files, run `bun install --cwd /workspace/src/typescript/shared/ui` before app install.

### Pain points addressed

- Triplicated configs → consolidated (FR-002).
- No documented shared-UI public surface → documented (FR-001).
- Type drift between `types/` and API contracts → contracts imported from `007/008` (User Story 3 implies single source).

### Pain points deferred

- Visual harness/Storybook → separate slice if pursued.

### Key Entities

- **Shared UI public component**: a typed component exported from the package index.
- **API base URL**: env-injected URL consumed via shared client.

## Success Criteria

- **SC-001**: `@banana/ui` ships an `index.ts` with explicit re-exports.
- **SC-002**: Repo contains exactly one `tailwind.config.ts` and one `vite.config.ts` per package (no `.js`/`.d.ts` duplicates committed).
- **SC-003**: No hardcoded API base URL strings in app source.
- **SC-004**: Cross-platform components labeled in `@banana/ui` README.
