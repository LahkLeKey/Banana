# Feature Specification: Per-Channel Design System Adoption

**Feature Branch**: `[028-per-channel-design-system-adoption]`
**Created**: 2026-04-26
**Status**: GATED. Hard prerequisite: slices 026 + 027.
**Source**: [../021-frontend-shared-design-system-spike/analysis/followup-readiness-channel-adoption.md](../021-frontend-shared-design-system-spike/analysis/followup-readiness-channel-adoption.md)

## Goal

Migrate React, Electron, and React Native consumers to import the v2
components from `shared/ui` and stop hard-coding colors / spacing in
channel code.

## Functional requirements

- **FR-028-01**: No remaining color / spacing hex / px literals in
  React, Electron, or React Native source files (except
  documented-in-code exceptions).
- **FR-028-02**: React loads `@banana/ui/tokens/web.css` so CSS
  variables exist at root.
- **FR-028-03**: Visual diff for verdict / chat / ripeness screens
  shows zero pixel-perceivable change.
- **FR-028-04**: All channel tsc + test gates remain clean.
- **FR-028-05**: Electron smoke remains green.

## Out of scope

- New components (slice 027).
- Token contract changes (slice 026).
- Dark mode (D-021-01).
- New customer journeys (SPIKE 020 owned them).

## Validation lane

```
bun run --cwd src/typescript/react tsc --noEmit
bun run --cwd src/typescript/react test
bun run --cwd src/typescript/react-native tsc --noEmit
bun run --cwd src/typescript/shared/ui tsc --noEmit
docker compose --profile apps run --rm electron-example npm run smoke
```

## Success criteria

- All FR-028-* hold.

## In-scope files

See [readiness packet](../021-frontend-shared-design-system-spike/analysis/followup-readiness-channel-adoption.md#in-scope-files).
