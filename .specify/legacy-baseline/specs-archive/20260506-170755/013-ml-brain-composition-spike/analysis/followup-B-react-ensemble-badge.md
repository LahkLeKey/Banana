# Follow-up B -- React `BananaBadge` ensemble variant

## Objective

Surface the `EnsembleVerdictResult` shape from slice 014 in the React UI by
adding an ensemble-aware variant to the existing `BananaBadge` shared
component, plus an optional attention chip behind a feature flag.

## In-scope files

- `src/typescript/shared/ui/src/components/BananaBadge.tsx` (extend, no
  rename; add `variant="ensemble"` prop)
- `src/typescript/shared/ui/src/types.ts` (extend with `EnsembleVerdict`
  type matching the managed payload)
- `src/typescript/shared/ui/src/components/BananaBadge.test.tsx` (NEW)
- `src/typescript/react/src/lib/api.ts` (add ensemble fetch helper)
- `src/typescript/react/src/lib/api.test.ts` (extend with ensemble shape
  snapshot test)
- `src/typescript/react/src/App.tsx` (wire the new badge variant behind
  the existing `VITE_BANANA_API_BASE_URL` contract)

## Out of scope

- Any change to `src/native/**` or `src/c-sharp/**`.
- Electron preload bridge (deferred until a desktop-only signal is asked
  for; lives outside this slice).
- New Bun packages.
- Tailwind config changes (reuse existing `BananaBadge` styling primitives).

## Validation lane

```
cd src/typescript/shared/ui && bun test
cd src/typescript/react && bun test
```

Plus a manual smoke against a running 014 backend:
```
cd src/typescript/react && bun run dev
# Confirm ensemble badge renders for: clear-banana, decoy, ambiguous, empty.
```

## Contract risk

- **API consumer contract** -- depends on the `EnsembleVerdictResult`
  shape from slice 014. MUST NOT add fields the backend does not yet
  produce. Use a TypeScript type derived directly from the 014 DTO.
- **Feature flag** -- attention chip MUST default to off. Use an env
  flag (e.g. `VITE_BANANA_SHOW_ATTENTION=1`) to opt in; absent flag MUST
  produce a UI byte-identical to the non-ensemble variant.

## Anchor references

- The same six anchor payloads from
  [composition-strategy.md](./composition-strategy.md) are the manual
  smoke set; UI must produce the matching final label for each.

## Owner agent

`react-ui-agent`.

## Acceptance summary

- `BananaBadge variant="ensemble"` renders final label, regression
  magnitude, and (if flag on) an attention chip.
- Component test covers the four UI states: clear-banana, clear-not-banana,
  ambiguous-escalated, decoy-rejected.
- API helper test snapshots the ensemble payload shape so backend
  contract drift fails this lane.

## Spec branch (when promoted)

`015-react-ensemble-badge`
