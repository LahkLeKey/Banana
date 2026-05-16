# Quickstart: Player Controller Foundation

## 1. Build WASM Engine Artifacts

From repository root:

- `bash scripts/build-engine-wasm-emsdk.sh`

Expected result:

- Updated runtime artifacts in `src/typescript/react/public/wasm/`.

## 2. Run React Runtime Channel (Local Dev)

Preferred local loop for this slice:

- `cd src/typescript/react`
- `bun run dev --host 0.0.0.0 --port 5176`

Open in browser/webview:

- `http://localhost:5176/`

## 3. Validate Keyboard-Only Movement

- Press and hold W, A, S, D and Arrow keys.
- Confirm movement starts on keydown and stops on keyup.
- Confirm no mouse click causes movement.

## 4. Validate Custom Context Menu

- Right-click in viewport.
- Confirm custom action menu appears within viewport bounds.
- Confirm menu closes when:
  - clicking outside,
  - pressing Escape,
  - selecting Close.

## 5. Regression Expectations

- Right-click must never mutate movement vectors in this slice.
- Keyboard movement remains authoritative.
- Any mouse movement request belongs to a later dedicated spec.

## 6. Runtime Validation Checklist

- Confirm `spec-extension-preflight.sh --update-first --json` returns pass and no corrupted extensions.
- Confirm `spec-confidence-gate.sh` records confidence >= 80 before implementation continues.
- Validate keyboard movement start/stop behavior for both WASD and Arrow keys.
- Validate context menu open/close behavior (right-click open, outside click close, Escape close).
- Validate right-click events do not mutate movement vectors in runtime checks.
- Record evidence links and outcomes in `.specify/specs/048-player-controller-foundation/heartbeat-log.md`.

## 7. Validation Evidence (2026-05-10)

- Native runtime contract suite:
  - Command: `build/native-us3/tests/native/engine/banana_test_engine_runtime_contracts.exe`
  - Result: 6 passed, 0 failed.
  - Coverage highlights: right-click inert path, normalized mouse bridge inert under active keyboard input, terrain/tick contract stability.
- React behavior suite:
  - Command: `cd src/typescript/react && bun test src/pages/__tests__/GameEnginePage.controller-input.test.tsx src/pages/__tests__/GameEnginePage.context-menu.test.tsx src/pages/__tests__/GameEnginePage.controller-arbitration.test.tsx`
  - Result: 12 passed, 0 failed.
  - Coverage highlights: keyboard axis mapping, context menu bounds/open-close behavior, rapid mixed-input arbitration and blur reset.
- Spec boundary validation:
  - Command: `bash .specify/scripts/bash/validate-spec-boundaries.sh --spec .specify/specs/048-player-controller-foundation/spec.md`
  - Result: pass.
- Task traceability validation:
  - Command: `bash .specify/scripts/bash/validate-task-traceability.sh --tasks .specify/specs/048-player-controller-foundation/tasks.md --spec .specify/specs/048-player-controller-foundation/spec.md`
  - Result: pass (34 mapped tasks, drift findings 0).

## 8. MVP Handoff Notes

- Controller authority is keyboard-only (`WASD` + Arrow keys), with explicit neutral reset on key release and focus/visibility loss.
- Custom right-click menu is bounded, deterministic, and isolated from movement state changes.
- Mouse bridge functions in native runtime remain intentionally inert to preserve arbitration invariants for this slice.
- US1, US2, US3, and polish tasks are complete in `.specify/specs/048-player-controller-foundation/tasks.md`.
- Future mouse-driven movement must be introduced only via a separate approved spec slice per contract.
