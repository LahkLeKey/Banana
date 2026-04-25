# Implementation Plan: Electron Desktop (v2)

**Branch**: `010-electron-desktop` | **Date**: 2026-04-25 | **Spec**: [spec.md](./spec.md)

## Technical Context

**Language/Version**: Node.js + Electron.  
**Primary Dependencies**: Electron, `@banana/ui`, native bridge modules.  
**Target Platform**: Windows host + Docker Desktop + Ubuntu WSL2 / WSLg.  
**Constraints**: Direct WSLg rendering; exit code 42 on preflight failure; distro order preserved.

## Project Structure

```text
src/typescript/electron/
├── package.json + package-lock.json
├── main.js                 # main process bootstrap
├── preload.js              # exposes the safe surface to renderer
├── renderer/               # consumes @banana/ui (or builds against react app)
├── nativeBridge.js         # canonical IPC module
├── apiClient.js            # reads API base URL
├── smoke.js                # `npm run smoke` smoke entry
└── run-desktop.sh          # local non-container fallback
```

## Phasing

- **Phase 0**: Audit `nativeBridge.js` vs `nativeCli.js`; decide consolidation or roles.
- **Phase 1**: Tighten preload/main split; ensure renderer consumes shared UI public surface only.
- **Phase 2**: Verify WSLg direct rendering and exit code 42 paths.
- **Phase 3**: Reduce libGL surface area in Dockerfile; pin base digests.
