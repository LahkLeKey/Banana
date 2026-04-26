# Feature Specification: Electron Desktop (v2)

**Feature Branch**: `010-electron-desktop`  
**Created**: 2026-04-25  
**Status**: Draft  
**Parent**: [`005-v2-regeneration`](../005-v2-regeneration/spec.md)  
**Baseline**: [`legacy-baseline/electron-desktop.md`](../../legacy-baseline/electron-desktop.md)

## Summary

Regenerate the Electron desktop runtime with a clear `main` / `preload` /
`renderer` split, a single native bridge module, and a hard distinction
between the smoke channel and the desktop UI channel. Preserve the WSL2 + WSLg
direct-rendering contract and exit code `42` for preflight failures.

## User Scenarios & Testing

### User Story 1 — One-click desktop launch on Windows (Priority: P1)

As a developer on Windows, running `scripts/launch-container-channels-with-wsl2-electron.sh`
brings up the Electron desktop UI against Docker Desktop + Ubuntu WSL2 with
direct-to-WSLg rendering.

### User Story 2 — Smoke channel never confused with UI (Priority: P1)

As an operator, the compose `electron-example` smoke channel is clearly
labeled as a smoke container (`npm run smoke`) and never serves as the
desktop UI runtime.

### User Story 3 — Single native bridge module (Priority: P2)

As a maintainer, native IPC lives in one bridge module; `nativeCli.js` is
either folded in or has a documented distinct purpose.

### User Story 4 — Reproducible container build (Priority: P2)

As CI, the Electron container image builds reproducibly with `npm ci --omit=dev`
and `CXXFLAGS=-fpermissive` for current native modules.

### Edge Cases

- WSLg/Display unavailable → fail fast with remediation, exit `42`.
- Wrong distro selected → preserve order `BANANA_WSL_DISTRO` → `Ubuntu-24.04` → `Ubuntu`.

## Requirements

### Functional Requirements

- **FR-001**: v2 MUST keep `scripts/launch-container-channels-with-wsl2-electron.sh` (Windows entry) and `scripts/compose-electron-desktop-wsl2.sh` (Ubuntu entry).
- **FR-002**: v2 MUST keep exit code `42` for integration preflight failures.
- **FR-003**: v2 MUST keep strict direct container-to-WSLg rendering with actionable error messages on missing prerequisites.
- **FR-004**: v2 MUST keep distro selection order: `BANANA_WSL_DISTRO` → `Ubuntu-24.04` → `Ubuntu`.
- **FR-005**: v2 MUST keep compose `electron-example` as smoke (non-UI), with `npm run smoke`.
- **FR-006**: v2 SHOULD consolidate `nativeBridge.js` and `nativeCli.js` or document each role.
- **FR-007**: Container build MUST use `npm ci --omit=dev` and `CXXFLAGS=-fpermissive`.

### Hard contracts to preserve

- Windows + Ubuntu entry-point script names.
- Exit code `42` for preflight failures.
- WSLg direct-rendering contract.
- Distro selection order.
- Smoke channel name + `npm run smoke`.
- libGL fixes captured in `/memories/repo/electron-rendering-libgl.md`.

### Pain points addressed

- Smoke vs UI confusion → explicit FR-005 + docs (User Story 2).
- Native bridge duplication → FR-006.
- Mixed PMs cognitive load → noted; npm here intentionally because `package-lock.json` is required for `npm ci` reproducibility (deferred change).

### Pain points deferred

- Mixed PM (npm here vs Bun in react/api) — keeping npm for `npm ci` lockfile semantics.
- Debian slim base highs — track as platform risk per parent contract.

## Success Criteria

- **SC-001**: One-click Windows launch produces the desktop UI in WSLg.
- **SC-002**: Compose smoke channel still passes `npm run smoke`.
- **SC-003**: Native bridge files reduced to 1 (or 2 with documented split).
- **SC-004**: Container build reproducible from clean cache.
