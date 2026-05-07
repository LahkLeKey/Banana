# Feature Specification: Electron Customer Verdict Surface

**Feature Branch**: `[024-electron-customer-verdict-surface]`
**Created**: 2026-04-26
**Status**: GATED. DO NOT EXECUTE until SPIKE 020 readiness packet is acknowledged.
**Source**: [../020-frontend-classifier-experience-spike/analysis/followup-readiness-electron.md](../020-frontend-classifier-experience-spike/analysis/followup-readiness-electron.md)
## Problem Statement

## In Scope

- Feature implementation and integration
- Testing and validation of new behavior
- Documentation of feature usage and design decisions

Feature Specification: Electron Customer Verdict Surface aims to improve system capability and user experience by implementing the feature described in the specification.


## Goal

Promote the Electron channel from smoke container to a real customer
surface: tray icon + menu, paste-classify global shortcut, native
notification on verdict completion. Reuses the React renderer for the
verdict panel UI; adds desktop-only entry points around it.

## Functional requirements

- **FR-024-01**: Tray icon visible at app start with menu entries
  "Classify clipboard" and "Show last verdict" using baseline copy.
- **FR-024-02**: Global shortcut `Cmd/Ctrl+Shift+B` triggers
  paste-classify (reads clipboard, sends to renderer, shows verdict).
- **FR-024-03**: Native notification renders the canonical verdict
  copy on completion (title = verdict label; body = baseline copy).
- **FR-024-04**: Errors surface inside the renderer toast using
  baseline error wording, not via native dialog.
- **FR-024-05**: Tray "Show last verdict" surfaces the last
  in-session verdict (cross-session history is SPIKE 022 / slice 030).
- **FR-024-06**: Smoke check passes in compose with the new bridge.

## Out of scope

- Drop-to-classify image ingest (deferred D-020-01).
- Tokens / theming (SPIKE 021).
- Offline / queueing / cross-session history (SPIKE 022).
- Auto-launch on login, multi-window, full menu bar redesign.

## Validation lane

```
docker compose --profile apps run --rm electron-example npm run smoke
bun run --cwd src/typescript/react tsc --noEmit
```

## Success criteria

- All FR-024-* requirements pass manual customer walkthrough.
- Smoke check passes in compose.
- React renderer remains tsc-clean.

## In-scope files

See [readiness packet](../020-frontend-classifier-experience-spike/analysis/followup-readiness-electron.md#in-scope-files).
