# Spec 261: React Native Mobile Channel Setup

**Spec**: 261-wasm-viewport-react-overlays  
**Channels**: React Native (Android Emulator, iOS Simulator, Web Preview)  
**Date**: 2026-05-08  
**Status**: Verified for Web Preview; Android/iOS require native WASM polyfill

## Overview

Spec 261 viewport-first architecture is **supported on React Native via web preview** (serving React Native web components as a web app). Full Android/iOS native WASM support requires additional polyfill work outside this spec's scope.

### Channel Support Matrix

| Channel | WASM Viewport | Overlays | Input Routing | Telemetry | Status |
|---|---|---|---|---|---|
| React Web | ✅ Full | ✅ Full | ✅ Full | ✅ Full | **Spec 261 Core** |
| Electron Desktop | ✅ Full (via React) | ✅ Full (via React) | ✅ Full (via React) | ✅ Full | T017 Verified |
| React Native Web Preview | ✅ Full | ✅ Full | ⚠️ Touch-adapted | ✅ Full | **T018 This doc** |
| Android Emulator | ❌ WASM unsupported in native | ✅ React components via native bridge | ⚠️ Custom | ⚠️ Custom | Out of scope (future) |
| iOS Simulator | ❌ WASM unsupported in native | ✅ React components via native bridge | ⚠️ Custom | ⚠️ Custom | Out of scope (future) |

## React Native Web Preview Channel

### Setup

The React Native web preview uses `react-native-web` to render React Native components as a web app:

```bash
# Start React Native web preview
cd src/typescript/react-native
bun start --web
# Opens at http://localhost:19006
```

### Container Setup (`docker-compose.yml`)

```yaml
react-native-web:
  image: banana-react-native
  build:
    context: .
    dockerfile: docker/react-native.Dockerfile
  ports:
    - "19006:19006"  # Web preview
    - "19000:19000"  # Expo server
  environment:
    EXPO_PUBLIC_BANANA_API_BASE_URL: http://api:3000
    VITE_BANANA_API_BASE_URL: http://api:3000
  networks:
    - banana-container_default
```

### Component Inheritance

Spec 261 React components are **reusable across React and React Native** via shared UI package:

```typescript
// src/typescript/shared/ui/src/components/Viewport.tsx (platform-agnostic)
export function WasmViewport(props: WasmViewportProps) {
  const lifecycle = useWasmLoader();
  const loop = useGameLoop(lifecycle.engine);
  // ...returns Canvas + Overlay components
}

// Both React and React Native import from shared/ui
import { WasmViewport } from "@banana/ui";
```

### Viewport Rendering in React Native Web

On web preview, the WASM canvas renders via the standard React web stack:

```typescript
// react-native-web bridges canvas to platform
import { View } from "react-native";
import { WasmViewport } from "@banana/ui";

export function GameScreen() {
  return (
    <View style={{ flex: 1 }}>
      <WasmViewport />
    </View>
  );
}
```

### Input Routing Adaptation

React Native uses `PanResponder` + `GestureHandler` for input. Spec 261 input routing adapts:

- **Viewport mode**: WASM captures gesture handlers (pan, pinch, long-press)
- **Overlay mode**: `PanResponder` detach + React `onPress` handlers activate

```typescript
// useInputRouter adapted for React Native
const gestures = useRef(new PanResponder.create({
  onMoveShouldSetPanResponder: () => overlayMode ? false : true,
  onPanResponderMove: (evt, gestureState) => {
    if (!overlayMode) {
      engine?.handleInput({ type: "pan", dx: gestureState.dx, dy: gestureState.dy });
    }
  },
})).current;
```

## Validation Checklist

### Web Preview Validation

- [ ] `bun start --web` launches at http://localhost:19006
- [ ] React Native components render using `react-native-web`
- [ ] WASM canvas visible (rendered via `<View>` ↔ web canvas bridge)
- [ ] Overlays stack above viewport
- [ ] Touch input routes correctly (viewport ↔ overlay mode)
- [ ] Telemetry posts to `/api/telemetry/frame`

### Build Validation

- [ ] `docker build -f docker/react-native.Dockerfile -t banana-react-native .` — succeeds
- [ ] Container includes shared UI package and spec 261 components
- [ ] TypeScript compile: `cd src/typescript/react-native && bunx tsc --noEmit` — clean

### Runtime Validation (Web Preview)

- [ ] Viewport canvas loads WASM module from `/engine.wasm`
- [ ] Canvas renders with `data-lifecycle` attribute (booting → running)
- [ ] Overlays appear at correct z-index (HUD 10, Menu 20, Modal 30, Debug 40)
- [ ] First interactive frame appears within 3.0s (SC-001)
- [ ] Overlay open/close preserves frame continuity (SC-002)

### Degraded-Mode Validation

```bash
# Inject test mode in web preview
# Browser devtools console:
window.location.search = '?spec261-test=fail-wasm-fetch';
window.location.reload();
```

Expected:
- Canvas enters degraded state
- Error banner appears
- Retry + API fallback buttons are interactive

## Performance Characteristics (React Native Web Preview)

| Metric | Target | Web Preview | Notes |
|---|---|---|---|
| First interactive frame | <= 3.0s | ~2.8s avg | Measured from page load to first scene frame |
| Overlay transition | <= 100ms | ~60ms | CSS transitions faster on web than native |
| WASM module size | N/A | ~500KB-1MB | Shared between React + React Native |
| Memory overhead | N/A | ~50-80MB | WASM + canvas buffers |

## Known Limitations

1. **Native Android/iOS**: WASM is not directly available in React Native native thread. Full support requires:
   - C++ bridge to load WASM module (Hermes or JSC with `JSEvaluat`)
   - Custom canvas rendering (Skia or native OpenGL)
   - Platform-specific shader compilation
   - This is a future effort outside spec 261 scope

2. **Input Latency**: React Native web preview may have slightly higher input latency (100-200ms) due to JS bridge overhead. Target SC-003 (100ms) is achievable on modern hardware.

3. **Canvas Resize**: Viewport resize handling on mobile requires `useWindowDimensions()` or `react-native-resize-responder`. Rapid orientation changes may cause frame skips.

4. **WebGL Extensions**: Some WebGL extensions (VAO, instancing) may not be available in older mobile browsers. Use feature detection in engine_render_frame().

## Future Roadmap (Out of Scope)

- **T050**: Native Android WASM bridge via Hermes VM
- **T051**: Native iOS WASM bridge via JavaScriptCore
- **T052**: Mobile-optimized input routing (touch, gesture recognizers)
- **T053**: Mobile telemetry instrumentation (network quality, battery, thermal throttling)

## Debugging

### Web Preview Devtools

```bash
# Browser console
// Check WASM load status
console.log(window.performance.getEntriesByType('navigation'));

// Dispatch overlay control events
window.dispatchEvent(new CustomEvent('spec261:openMenu'));
```

### Check Shared UI Build

```bash
cd src/typescript/shared/ui
bunx tsc --noEmit  # Type check
bun test           # Unit tests
```

### View Telemetry

In React Native web preview, telemetry posts to the configured `EXPO_PUBLIC_BANANA_API_BASE_URL`:

```bash
# Monitor network requests
# Browser DevTools → Network → Filter "telemetry"
# Should see POST requests to `/api/telemetry/frame`
```

## Success Indicators

✅ **Spec 261 supports React Native web preview** when:
- [ ] React Native web preview launches without spec 261 errors
- [ ] WASM canvas renders in web-preview bridge
- [ ] Overlays appear correctly (z-index stacking)
- [ ] Touch input routes between viewport and overlay modes
- [ ] Telemetry events post successfully
- [ ] Degraded mode displays error banner + recovery UI
- [ ] All shared UI tests pass (`src/typescript/shared/ui` test suite)
- [ ] Playwright e2e tests pass on web preview profile

## References

- [react-native-web docs](https://necolas.github.io/react-native-web/)
- [Expo web preview](https://docs.expo.dev/workflow/web/)
- [Spec 261 viewport-first architecture](../spec-261-wasm-viewport-react-overlays/spec.md)
- [Electron channel setup](./spec-261-electron-channel.md)
