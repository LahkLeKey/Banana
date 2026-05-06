# Slice 028 -- Per-Channel Design System Adoption

**Branch**: `feature/028-per-channel-design-system-adoption`
**Status**: COMPLETE.
**Hard prerequisite**: slices 026 + 027.

## Summary

- React app (`src/typescript/react`) now imports
  `@banana/ui/tokens/web.css` from `index.css`, so the slice 026 CSS
  variables exist at `:root` for any consumer style.
- React `App.tsx` swapped its inline ensemble error/retry surface
  to the v2 primitives `ErrorText` and `RetryButton` from
  `@banana/ui` (preserving `data-testid="ensemble-error"` /
  `data-testid="ensemble-retry"` via the spread-rest passthrough).
  The remaining `text-red-700` `<p>` callsites for `ripenessError`
  and `chatError` also moved to `<ErrorText>`.
- React Native screens (`CaptureScreen`, `VerdictScreen`) replaced
  inline color/spacing hex/px literals with `tokens.color.*`,
  `tokens.space.*`, `tokens.font.*`, `tokens.radius.*`. A small
  number of channel-specific hex values (lime "Classify" CTA, sky
  "Try again" CTA, neutral input border) are kept with inline
  `// Slice 028 exception:` comments documenting that no matching
  token exists yet (FR-028-01 documented exception path).
- `@banana/ui` package now exports `./tokens/web.css` so the React
  app can `@import` it; the React Native barrel additionally
  re-exports `tokens` / `Tokens` so the screens can consume token
  literals without deep imports (FR-028-02 + slice 027 contract).

## Acceptance criteria

- FR-028-01: Hex/px literals in React + RN sources removed except
  for inline-documented CTA color exceptions.
- FR-028-02: React loads the generated `tokens/web.css` at root.
- FR-028-03: Visual diff -- the token contract resolves to the same
  palette slices 023/024/025 already validated; no copy / cue
  changes were made.
- FR-028-04: tsc clean across `shared/ui`, `react`, `react-native`;
  React `bun test` 25/0; shared/ui `bun test` 13/0.
- FR-028-05: Electron entry points are JS-only and untouched in
  this slice; smoke contract unchanged from slice 024.

## Validation lane

```sh
cd src/typescript/shared/ui && bunx tsc --noEmit && bun test
cd src/typescript/react      && bunx tsc --noEmit && bun test
cd src/typescript/react-native && bunx tsc --noEmit
bash scripts/validate-spec-tasks-parity.sh --all
```

Results:

- shared/ui: tsc clean; `13 pass / 0 fail`.
- react: tsc clean; `25 pass / 0 fail`.
- react-native: tsc clean.
- spec/tasks parity: exit 0.

## Documented exceptions

The following hex literals are intentionally retained in this slice
(see inline `// Slice 028 exception:` comments):

- `#cbd5e1` neutral input border in `CaptureScreen` -- no border
  token shipped in slice 026.
- `#65a30d` lime "Classify" CTA in `CaptureScreen` -- no
  CTA-tone token shipped.
- `#0ea5e9` sky "Try again" CTA in `VerdictScreen` -- ditto.

Closing these is a follow-up after the design tokens contract grows
border + CTA tones (deferred backlog item).
