# Quickstart: ARPG View and Actor Scaffolding

## Goal

Validate centered ARPG framing, finished context-menu interactions, and non-player actor behavior in one local runtime loop.

## Prerequisites

- Branch: `049-arpg-view-actors`
- Active feature pointer: `.specify/feature.json` -> `.specify/specs/049-arpg-view-actors`
- Docker available for containerized wasm build (or local emsdk installed)
- Bun dependencies installed for `src/typescript/react`

## Steps

1. Build fresh wasm engine artifacts.

```bash
MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' docker run --rm -v /c/Github/Banana:/workspace -w /workspace emscripten/emsdk bash scripts/build-engine-wasm.sh --release
```

2. Start React runtime shell.

```bash
cd src/typescript/react
bun run dev --host 127.0.0.1 --port 5173
```

3. Open runtime in the VS Code integrated browser.

- URL: `http://127.0.0.1:5173/`

4. Validate ARPG framing behavior.

- Move with WASD and Arrow keys.
- Confirm player remains in centered follow region across viewport resize.
- Confirm terrain feels centered rather than corner-anchored.

5. Validate context-menu behavior.

- Right-click actor and verify target-aware action list.
- Confirm action selection closes menu and applies visible outcome.
- Confirm Escape/outside click closes menu without movement deadlock.

6. Validate non-player actors.

- Confirm non-player actors spawn consistently on load.
- Confirm interactions route only to selected actor when multiple actors are visible.

## Suggested Verification Commands

```bash
cd /c/Github/Banana
bun test src/typescript/react/src/pages/__tests__/GameEnginePage.context-menu.test.tsx
```

## Completion Criteria

- ARPG framing remains stable in local runtime checks.
- Context-menu interaction loop is actionable and target-scoped.
- Non-player actor presence and interaction outcomes are repeatably verified.

## Validation Evidence

- 2026-05-12: Rebuilt wasm via Dockerized `scripts/build-engine-wasm.sh --release` flow.
- 2026-05-12: Started React dev server at `http://127.0.0.1:5173/` and verified browser runtime in the VS Code integrated browser.
- 2026-05-12: Right-clicked canvas and selected `Interact`; runtime feedback returned `interact: applied to nearest actor.`

## Runtime Verification Notes

- Always rebuild wasm artifacts before visual verification to avoid stale engine behavior.
- If local `emcc` is unavailable, use the containerized build command in Step 1.
- If the scene fails to update while controls render, verify exported wasm bridge symbols in `scripts/build-engine-wasm.sh` include movement and interaction entrypoints.
