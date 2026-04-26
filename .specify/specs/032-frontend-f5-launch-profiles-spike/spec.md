# Feature Specification: Frontend F5 Launch Profiles SPIKE

**Feature Branch**: `[032-frontend-f5-launch-profiles-spike]`
**Created**: 2026-04-26
**Status**: Draft -- ready to investigate
**Type**: SPIKE (investigate -> plan a small family of bounded follow-ups)
**Triggered by**: After landing the customer-value cascade (slices
023-031) the local frontend story is "open three terminals, run
three commands, hope ports line up". A one-key F5 entry would
shrink the inner loop for contributors and demos. Mobile + desktop
runtime contracts already live in `.github/instructions/` and in
`scripts/launch-container-channels-with-wsl2-*.sh`; the gap is the
**non-container, host-side** developer launch.

## Problem

Each frontend channel today has its own launch incantation:

- React (`bun --cwd src/typescript/react run dev`)
- Electron (`node smoke.js` or `npm start` from the electron dir)
- React Native (`bunx expo start --web` from the rn dir)
- API (`bun --cwd src/typescript/api run dev`)

Contributors must remember the cwd, the env vars (`VITE_*`,
`EXPO_PUBLIC_*`, `BANANA_API_PORT`), and the fact that each
frontend wants the API up first. There is no `.vscode/launch.json`
checked in today, which means there is no F5 entry point and no
shared compound profile that boots API + a single channel
together.

## Investigation goals

- **G1**: Inventory the existing host-side launch surface
  (package.json scripts, container scripts, runtime contract
  docs). Output: launch-surface matrix (channel x mode x
  prerequisites).
- **G2**: Decide the minimum viable F5 contract:
  - Per-channel single-launch entries (API alone, React alone,
    etc.).
  - Compound entries that boot API + one frontend.
  - Whether to wrap container scripts as VS Code tasks vs leaving
    them shell-only.
- **G3**: Decide the env-var seam. Either:
  - Hard-code defaults in `launch.json` (`VITE_BANANA_API_BASE_URL`
    = `http://localhost:8080`).
  - Read from a `.env.local` (per-channel).
  - Use VS Code input variables.
- **G4**: Confirm the launch profiles do not regress the
  Ubuntu/WSL2 container contract: F5 is host-side only; the
  container scripts remain the canonical reproducible runtime.

## Concrete deliverable in this SPIKE

Because the F5 contract is small, this SPIKE ships an initial
`.vscode/launch.json` with exactly four single-channel
configurations (`API: Bun dev`, `React: Vite dev`,
`Electron: smoke`, `React Native: Expo web`) and three compound
entries (`API + React`, `API + Electron`, `API + React Native`).
Anything beyond that family is deferred to slice 035 (or whichever
follow-up the analysis recommends).

## Out of Scope

- Debugger attach (Chromium devtools, Electron renderer). Defer
  with explicit trigger.
- Container-side launch parity (already owned by
  `scripts/launch-container-channels-with-wsl2-*.sh`).
- Mobile emulator launch (owned by 033 + the existing mobile
  runtime contract).
- Test runner launches (`bun test`, `dotnet test`). Defer to
  follow-up if requested.

## Deliverables

- `.vscode/launch.json` (NEW) -- the four single + three compound
  entries described above.
- `analysis/launch-surface-matrix.md` -- G1 output.
- `analysis/f5-contract-decision.md` -- G2 + G3 decisions with
  rationale.
- `analysis/followup-readiness.md` -- bounded scope for any
  follow-up slice (debugger attach, test runner launches, etc.).

## Success Criteria

- F5 from a fresh clone boots `API + React` end-to-end without
  manual env editing.
- Compound entries stop cleanly (`stopAll: true`).
- Container runtime contract docs still describe the canonical
  reproducible path; launch.json is documented as **host-side
  developer convenience** only.
- Spec/tasks parity passes.

## Validation lane

```
bash scripts/validate-spec-tasks-parity.sh --all
# Manual: F5 -> "Frontends: API + React" -> hit http://localhost:5173
```

## Downstream

- 033 Training Launch Profiles SPIKE -- adds Python `debugpy`
  entries for the trainers (already stubbed in this slice's
  launch.json under the `training` group; 033 expands the surface).
- 035 Frontend F5 Debugger Attach (deferred; explicit trigger =
  contributor asks for breakpoints in renderer).

## In-scope files

- `.vscode/launch.json` (NEW).
- `.specify/specs/032-frontend-f5-launch-profiles-spike/**` (NEW).
- `.specify/feature.json` (repoint).
