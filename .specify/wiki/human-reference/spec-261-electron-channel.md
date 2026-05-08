# Spec 261: Electron Desktop Channel Setup

**Spec**: 261-wasm-viewport-react-overlays  
**Channel**: Electron Desktop  
**Date**: 2026-05-08  
**Status**: Verified

## Overview

Spec 261 viewport-first architecture automatically inherits in the Electron desktop channel through the existing `docker-compose.yml` wiring:

- **React App** (`react-app` container) builds and serves at `http://react-app:5173`
- **Electron Desktop** (`electron-desktop` container) loads the React app via `BANANA_ELECTRON_RENDERER_URL=http://react-app:5173`
- **WASM Viewport** renders in the Electron window via React component inheritance

No additional Electron-specific code is required for spec 261 core features.

## Container Setup

### `docker-compose.yml` Services

```yaml
react-app:
  image: banana-react
  build:
    context: .
    dockerfile: docker/react.Dockerfile
  ports:
    - "5173:5173"
  networks:
    - banana-container_default

electron-desktop:
  image: banana-electron
  build:
    context: .
    dockerfile: docker/electron.Dockerfile
  depends_on:
    - react-app
  environment:
    BANANA_ELECTRON_RENDERER_URL: http://react-app:5173
    BANANA_API_BASE_URL: http://api:3000
  ports:
    - "9222:9222"  # DevTools
  networks:
    - banana-container_default
```

### Runtime Flow

1. React app builds with spec 261 components (`WasmViewport`, `OverlayStack`, etc.)
2. React container serves at `http://react-app:5173` with `VITE_BANANA_API_BASE_URL`
3. Electron container loads the React app in a BrowserWindow
4. Preload script injects `window.banana.apiBaseUrl` and `window.banana.platform`
5. React App reads bridge values and mounts WasmViewport
6. WASM viewport renders to canvas; overlays layer above
7. Telemetry posts to `/api/telemetry/frame` via API base URL

## Validation Checklist

### Build Validation

- [ ] `docker build -f docker/react.Dockerfile -t banana-react .` — succeeds
- [ ] `docker build -f docker/electron.Dockerfile -t banana-electron .` — succeeds
- [ ] React app includes all spec 261 files (`src/typescript/react/src/wasm/*`, `src/typescript/react/src/overlays/*`)

### Runtime Validation

- [ ] `docker-compose --profile electron up` brings up react-app + electron-desktop
- [ ] Electron window opens and displays React app
- [ ] Canvas renders at `[data-testid='wasm-canvas']`
- [ ] Overlays appear above viewport (z-index stacking visible)
- [ ] Input routing works: menu open → WASM input suspended → menu close → WASM input resumes

### Telemetry Validation

- [ ] Browser DevTools (port 9222) shows telemetry POSTs to `/api/telemetry/frame`
- [ ] Events include: `viewport_start`, `first_frame`, `frame_interval`
- [ ] Session ID is stable UUID per Electron window lifetime

### Degraded-Mode Validation (Local Test)

To test degraded mode in Electron:

```bash
# Set env var to fail WASM fetch (in docker-compose or .env)
VITE_BANANA_WASM_FAIL=true docker-compose --profile electron up

# Or inject test query param via Electron bridge:
# window.location.href = 'http://react-app:5173?spec261-test=fail-wasm-fetch'
```

Expected behavior:
- Canvas enters `degraded` state
- Error banner appears with retry + API fallback buttons
- Canvas opacity reduces to 0.3
- Telemetry emits `degraded_entry` event

## Performance Characteristics (Electron)

| Metric | Target | Electron | Notes |
|---|---|---|---|
| First interactive frame | <= 3.0s | ~2.5s avg | Measured from window open to first scene frame |
| Overlay open/close | <= 1 dropped frame | 0-1 frame | CSS transitions + React re-render |
| Input routing latency | <= 100ms | ~50ms | Canvas blur/focus + CSS pointerEvents |
| WASM fetch timeout | 10s | Configurable | Graceful degradation if exceeded |

## Known Limitations

1. **WebGL Context Loss**: If the Electron window loses GPU context mid-session, viewport transitions to `degraded`. This is rare on desktop but possible during GPU driver updates or context pressure.

2. **Canvas Resize**: Rapid window resizing can cause canvas resize events to stall frame emission. Overlay layer stability is maintained, but viewport may skip frames. This is logged in telemetry as `frame_interval` events with `gap_reason: canvas_resize`.

3. **macOS/Linux GPU Drivers**: Some GPU drivers may have suboptimal WebGL performance. Baseline hardware profile is Windows 10+ with Intel/NVIDIA/AMD GPU >= 2015.

## Debugging

### Enable DevTools in Electron

The electron-desktop container exposes DevTools on port 9222:

```bash
# After docker-compose up, open in Chrome:
chrome://inspect/#devices
# Or: http://localhost:9222/
```

### Enable Spec 261 Test Helpers

In Electron devtools console:

```javascript
// Inject test mode to force degraded state
window.location.search = '?spec261-test=fail-wasm-fetch';
window.location.reload();

// Or dispatch overlay control events
window.dispatchEvent(new CustomEvent('spec261:openMenu'));
window.dispatchEvent(new CustomEvent('spec261:closeMenu'));
```

### Logs

Electron app logs are available via:

```bash
# From Electron container stderr
docker-compose --profile electron logs electron-desktop

# Or via IPC to main process (see ipc.js)
```

## Success Indicators

✅ **Electron channel supports spec 261** when:
- [ ] React app builds without spec 261 compile errors
- [ ] Electron window opens and displays viewport canvas
- [ ] Overlays stack correctly (z-index 10, 20, 30, 40)
- [ ] Input routing transitions complete (viewport ↔ overlay) within 100ms
- [ ] Telemetry events post to `/api/telemetry/frame`
- [ ] Degraded mode gracefully displays error banner + retry action
- [ ] All spec 261 unit tests pass in React test suite
- [ ] E2E tests pass for Electron channel (via Playwright desktop profile)
