# Slice 027 -- Shared UI Components v2

**Branch**: `feature/027-shared-ui-components-v2`
**Status**: COMPLETE.
**Hard prerequisite**: slice 026 (tokens + theme primitives).

## Summary

- Refactored existing shared web components (`BananaBadge`,
  `RipenessLabel`, `ChatMessageBubble`, `EscalationPanel`) to consume
  the slice 026 token contract via inline styles + token literals.
  Public props preserved; class-based Tailwind palette literals
  replaced with token-resolved values.
- Refactored existing shared React Native components
  (`BananaBadge`, `ChatMessageBubble`, `RipenessLabel`,
  `EscalationPanel`) to import `tokens` from `../tokens` and drop
  hard-coded hex literals.
- Closed inventory gap: `BananaBadge` (web + native) now exposes an
  `ensemble` variant (label + magnitude + escalation pill +
  optional attention chip) mirroring the React app's anchor screen.
- Added new canonical primitives in both web (`components/`) and
  native (`native/`) folders: `RetryButton` (baseline copy
  `"Try again"`), `ErrorText` (alert paragraph in
  `tokens.color.text.error`).
- Updated public surface (`src/index.ts` and `src/native/index.ts`)
  to export the new components and prop types.

## Acceptance criteria

- FR-027-01 (token consumption + render parity): all touched
  components consume tokens; consumer apps continue to render.
- FR-027-02 (native ensemble parity): native `BananaBadge` exposes
  the same ensemble variant fields the web variant exposes (label,
  magnitude, escalation, attention).
- FR-027-03 (native ripeness coverage): native `RipenessLabel` is
  token-driven (file already existed; refactored, not added).
- FR-027-04 (canonical Retry/Error primitives): `RetryButton` and
  `ErrorText` shipped in both targets.
- FR-027-05 (public surface contract): `src/index.ts` and
  `src/native/index.ts` re-export every new symbol; deep imports
  remain forbidden.

## Pre-existing files note

The 027 task list assumed `EscalationPanel` (web/native) and the
native `BananaBadge`/`ChatMessageBubble`/`RipenessLabel` files were
gaps. They already shipped with slices 023/025 with hard-coded
literals; this slice refactored them in place rather than creating
new files.

## Hex resolution note

Slice 026 locked the token palette to specific hex values (e.g.
`color.banana.bg = #fef3c7`). Some pre-027 consumers used different
shades (e.g. Tailwind `yellow-200 = #fef08a`, native badge bg
`#fde68a`). The token-driven refactor resolves the design contract
to slice 026's palette; minor visual deltas vs. the pre-026 baseline
are intentional and per contract. Per-channel adoption sweep lands
in slice 028.

## Validation lane

```sh
cd src/typescript/shared/ui && bunx tsc --noEmit && bun test
cd src/typescript/react      && bunx tsc --noEmit && bun test
cd src/typescript/react-native && bunx tsc --noEmit
bash scripts/validate-spec-tasks-parity.sh --all
```

Results:

- shared/ui: tsc clean; `13 pass / 0 fail` (2 token contract +
  EscalationPanel 5 + RetryButton 4 + ErrorText 2).
- react: tsc clean; `25 pass / 0 fail`.
- react-native: tsc clean.
- spec/tasks parity: exit 0.
