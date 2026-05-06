# In-scope files -- 015 React Ensemble Badge

## Source under change

- `src/typescript/shared/ui/src/types.ts` (new `EnsembleVerdict` type)
- `src/typescript/shared/ui/src/index.ts` (re-export new types)
- `src/typescript/shared/ui/src/components/BananaBadge.tsx` (add
  `variant="ensemble"`, escalation pill, opt-in attention chip)
- `src/typescript/react/src/lib/api.ts` (new `fetchEnsembleVerdict` helper)
- `src/typescript/react/src/App.tsx` (wire ensemble form + badge)

## Tests under change

- `src/typescript/react/src/BananaBadge.test.tsx` (NEW; component tests
  using `react-dom/server` -- placed in the react package because shared/ui
  has no test runner deps and slice 015 forbids new Bun packages)
- `src/typescript/react/src/lib/api.test.ts` (new ensemble snapshot test)

## Out of scope

- Native or ASP.NET (`src/native/**`, `src/c-sharp/**`)
- Electron preload bridge
- New Bun packages
- Tailwind config changes
