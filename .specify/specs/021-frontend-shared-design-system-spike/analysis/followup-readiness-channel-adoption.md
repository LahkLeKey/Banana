# 021 -- Followup Readiness: Slice 028 (Per-Channel Design System Adoption)

Bounded scope packet for the per-channel adoption slice.

**Hard prerequisite**: slices 026 and 027 have landed.

## Goal

Migrate React, Electron, and React Native consumers to import the v2
components from `shared/ui` and stop hard-coding colors / spacing in
channel code.

## In-scope files

- `src/typescript/react/src/App.tsx` (replace inline Tailwind colors
  for verdict / error surfaces with token-driven classes via
  v2 components)
- `src/typescript/react/src/index.css` (load
  `@banana/ui/tokens/web.css` so CSS variables exist at root)
- `src/typescript/electron/main.js` (no UI changes; verify tray icon
  uses token-derived asset path -- if the design lead chose a token
  for the tray asset, otherwise no-op)
- `src/typescript/react-native/index.tsx` (replace inline color hex
  literals with `tokens.color.*` references)
- `src/typescript/react-native/screens/CaptureScreen.tsx` (consume
  v2 components; adopted from slice 025)
- `src/typescript/react-native/screens/VerdictScreen.tsx` (same)
- vitest snapshot updates if any (acceptable as part of cutover)

## Out of scope

- New components (slice 027 owns).
- Token contract changes (slice 026 owns).
- Dark mode rollout.
- New customer journeys (SPIKE 020 owns).

## Validation lane

```
bun run --cwd src/typescript/react tsc --noEmit
bun run --cwd src/typescript/react test
bun run --cwd src/typescript/react-native tsc --noEmit
bun run --cwd src/typescript/shared/ui tsc --noEmit
docker compose --profile apps run --rm electron-example npm run smoke
```

## Estimated task count

12-15 tasks. Buckets: setup (1) + React adoption (4) + RN adoption
(4) + Electron sanity (1) + visual diff review (2) + close-out (2).

## Acceptance

- No remaining color / spacing hex / px literals in channel source
  files (except deferred-design exceptions documented in code).
- Visual diff for verdict / chat / ripeness screens shows zero
  pixel-perceivable change (token values match the literals they
  replace).
- All channel tsc + test gates remain clean.
- Electron smoke remains green.
