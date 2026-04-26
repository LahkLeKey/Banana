# Feature Specification: Electron Persistence + Retry

**Feature Branch**: `[030-electron-persistence-retry]`
**Created**: 2026-04-26
**Status**: GATED. Hard prerequisite: slice 029.
**Source**: [../022-frontend-resilient-offline-spike/analysis/followup-readiness-electron.md](../022-frontend-resilient-offline-spike/analysis/followup-readiness-electron.md)

## Goal

Adopt the shared resilience primitive in Electron. Add desktop-only
surfaces: window-position persistence, "show last verdict" tray
menu entry backed by `VerdictHistory.list()`, and a native
notification on queued-then-drained verdicts.

## Functional requirements

- **FR-030-01**: Closing and reopening the Electron app restores
  window position and size.
- **FR-030-02**: Tray "Show last verdict" surfaces the most-recent
  stored verdict using canonical baseline copy.
- **FR-030-03**: A queued-while-offline verdict surfaces a native
  notification when it drains successfully.
- **FR-030-04**: Renderer continues to use the shared primitive's
  IndexedDB adapter (no main-process verdict storage).
- **FR-030-05**: Electron smoke remains green.

## Out of scope

- Verdict storage in main process.
- Cross-device sync (D-022-01).
- Auto-update.

## Validation lane

```
docker compose --profile apps run --rm electron-example npm run smoke
bun run --cwd src/typescript/react tsc --noEmit
```

## Success criteria

- All FR-030-* hold.

## In-scope files

See [readiness packet](../022-frontend-resilient-offline-spike/analysis/followup-readiness-electron.md#in-scope-files).
