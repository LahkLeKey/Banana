# 030 Electron Persistence + Retry -- Execution Tracker

**Branch**: `030-electron-persistence-retry`
**Source SPIKE**: [022 Frontend Resilient Offline](../022-frontend-resilient-offline-spike/spec.md)
**Status**: GATED. Hard prerequisite: slice 029.

## Phases (mapped to tasks.md)

1. Setup (T001-T002)
2. Window state persistence (T003-T004)
3. Tray history wiring (T005-T007)
4. Drain notification (T008-T009)
5. Smoke + close-out (T010-T012)

## Constraints

- Verdict storage stays in renderer (IndexedDB).
- Main-process storage scope = window state only.
