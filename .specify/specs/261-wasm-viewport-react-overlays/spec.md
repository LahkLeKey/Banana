# Feature Specification: 261 WASM Viewport + React Overlay Runtime

**Feature Branch**: `261-wasm-viewport-react-overlays`
**Created**: 2026-05-07
**Status**: Proposed
**Wave**: frontend
**Domain**: frontend

## In Scope

- Make the WASM viewport the primary full-screen runtime surface for the game scene.
- Define React as overlay-only UI (HUD, menu, modal, diagnostics, notifications) layered above the viewport.
- Define input-routing rules between scene controls and overlay interactions.
- Define viewport lifecycle states and deterministic fallback behavior when WASM cannot initialize.
- Define telemetry events that prove runtime responsiveness and overlay correctness.

## Out of Scope

- Replacing the native C engine architecture or ABI defined in spec 001.
- Introducing a full new rendering framework for the scene layer.
- Solving multiplayer networking, matchmaking, or world streaming.
- Rebuilding non-game product surfaces unrelated to runtime overlays.

## User Scenarios & Testing

### User Story 1 - Run In WASM Viewport First (Priority: P1)

As a player, I open the app and immediately see the game world rendered by the WASM viewport as the main background, with no React-owned scene rendering.

**Why this priority**: This is the core migration outcome. Without this, the project is not operating as a game-engine-first client.

**Independent Test**: Launch the web runtime and verify that scene frames are produced by the viewport while React renders only overlay containers.

**Acceptance Scenarios**:

1. **Given** the runtime loads successfully, **When** the first scene is ready, **Then** the viewport occupies the full background canvas and frame telemetry begins.
2. **Given** the runtime is active, **When** overlays mount/unmount, **Then** scene rendering continues and frame output remains visible.

---

### User Story 2 - Interact With React Overlays Without Breaking Scene Control (Priority: P2)

As a player, I can open HUD/menu overlays and interact with UI actions while scene controls pause, resume, or route deterministically.

**Why this priority**: Overlay usability is required for real gameplay flow and debugging workflows.

**Independent Test**: Trigger overlay open/close and confirm input routing transitions between viewport and UI are deterministic.

**Acceptance Scenarios**:

1. **Given** viewport control is active, **When** a pause menu opens, **Then** scene control input is suspended and UI receives pointer/keyboard focus.
2. **Given** an overlay closes, **When** focus returns to viewport mode, **Then** scene controls resume within one interaction cycle.

---

### User Story 3 - Recover Gracefully From WASM Runtime Failure (Priority: P3)

As an operator, I get clear degraded-mode behavior and diagnostics when viewport initialization fails or stalls.

**Why this priority**: Migration must be resilient and observable in CI, staging, and local runtime channels.

**Independent Test**: Simulate WASM init timeout/failure and verify React fallback overlay displays actionable recovery state without crashing app shell.

**Acceptance Scenarios**:

1. **Given** WASM startup fails, **When** runtime reaches timeout threshold, **Then** a degraded overlay appears with retry and API fallback options.
2. **Given** recovery succeeds after retry, **When** viewport returns healthy, **Then** fallback overlay exits and normal runtime telemetry resumes.

### Edge Cases

- WASM module loads but first frame never emits (boot hang).
- WebGL/WebGPU context loss mid-session.
- Rapid viewport resize/orientation changes while overlays are open.
- Input focus thrash between overlay modal and pointer-lock scene mode.
- Asset fetch latency causes delayed scene start while overlay shell is already interactive.

## Requirements

### Functional Requirements

- **FR-001**: The runtime MUST render the primary scene through a dedicated WASM viewport container that is visually beneath all React overlays.
- **FR-002**: React MUST NOT own or replace the scene frame render loop.
- **FR-003**: The runtime MUST provide explicit viewport lifecycle states: `booting`, `ready`, `running`, `degraded`, `recovering`, `stopped`.
- **FR-004**: The UI shell MUST support overlay layer priorities (HUD < modal < critical alert) with deterministic stacking rules.
- **FR-005**: Input routing MUST support deterministic transitions between viewport-control mode and overlay-interaction mode.
- **FR-006**: The runtime MUST emit telemetry for viewport start latency, first-frame latency, steady-state frame interval, overlay open/close events, and fallback transitions.
- **FR-007**: On WASM init timeout/failure, the app MUST enter degraded overlay mode without hard crash.
- **FR-008**: Degraded overlay mode MUST expose retry and backend-fallback actions with clear status messages.
- **FR-009**: React overlay actions that do not require scene control MUST remain usable while viewport is degraded.
- **FR-010**: The runtime MUST preserve compatibility with existing API persistence/telemetry endpoints used by game-state flows.
- **FR-011**: The migration MUST include contract tests for viewport lifecycle transitions and input-routing transitions.
- **FR-012**: CI evidence MUST include at least one healthy runtime path and one degraded recovery path.

### Key Entities

- **ViewportRuntimeState**: Canonical state machine for viewport lifecycle, readiness, and health transitions.
- **OverlayLayer**: Overlay definition including priority, visibility, interaction lock, and focus behavior.
- **InputRoutingPolicy**: Policy object defining whether keyboard/mouse/touch events target viewport or overlay.
- **RuntimeTelemetryEvent**: Structured telemetry payload for latency, failure, overlay transitions, and recovery outcome.
- **FallbackActionSet**: Retry, API fallback, and escalation actions exposed during degraded mode.

## Success Criteria

### Measurable Outcomes

- **SC-001**: In supported desktop browsers, first interactive frame appears in <= 3.0 seconds in 95% of runs on baseline hardware profile.
- **SC-002**: Overlay open/close operations preserve viewport continuity with no more than one dropped scene frame per transition under nominal load.
- **SC-003**: Input routing transitions (viewport -> overlay -> viewport) complete deterministically with <= 100 ms handoff latency.
- **SC-004**: 100% of simulated WASM initialization failures enter degraded mode with actionable recovery UI and telemetry evidence.
- **SC-005**: CI/UAT evidence includes one passing healthy-path run and one passing degraded-recovery-path run per release gate.

## Viewport Lifecycle State Machine (T001)

The `ViewportRuntimeState` canonical state machine governs all runtime transitions:

```
booting → ready → running ⇄ degraded → recovering → running
                  running → stopped
booting → degraded
```

| State | Entry Condition | Exit Condition | Allowed Actions |
|---|---|---|---|
| `booting` | Component mounts, WASM fetch begins | Fetch+instantiate success → `ready`; timeout/error → `degraded` | Cancel, observe progress |
| `ready` | WASM instantiated, first engine_tick pending | First frame emitted → `running`; frame timeout → `degraded` | Start loop |
| `running` | `requestAnimationFrame` loop active, frames emitting | Overlay lock → stays `running`; error → `degraded`; unmount → `stopped` | Tick, render, emit telemetry |
| `degraded` | WASM fetch failure, init timeout, context loss, tick exception | Retry action → `recovering`; API fallback selected → `stopped` | Retry, select fallback, read error |
| `recovering` | Retry initiated (re-fetch or re-instantiate) | Success → `ready`; failure → `degraded` | Cancel |
| `stopped` | Unmount or explicit shutdown or API fallback selected | Terminal — component unmounts | None |

**Timeout thresholds**:
- WASM fetch: 10 000 ms
- First frame: 5 000 ms after `ready`
- Tick exception retry budget: 3 consecutive failures before → `degraded`

**State transitions are synchronous** — no implicit delays. All side effects (telemetry, UI banner, loop stop) fire in the transition handler.

## Overlay Layer Priority and Focus-Lock Contract (T002)

`OverlayLayer` defines four named layers with deterministic stacking and focus-lock rules:

| Layer | z-index | Pointer Events | Focus Lock | Input Routing |
|---|---|---|---|---|
| `hud` | 10 | none (pass-through) | none | WASM captures all events |
| `menu` | 20 | auto on open | soft lock — viewport suspended | UI receives keyboard/pointer |
| `modal` | 30 | auto always | hard lock — WASM suspended | UI only; WASM input discarded |
| `debug` | 40 | none (pass-through) | none | WASM captures all events |

**Rules**:
1. At most one focus-lock layer may be active at a time. `modal` wins over `menu`.
2. When any focus-lock layer opens, `useInputRouter` transitions to `overlay` mode: canvas `blur()` is called and `pointerEvents: none` is set on the canvas.
3. When all focus-lock layers close, `useInputRouter` transitions back to `viewport` mode: canvas `focus()` is called.
4. `hud` and `debug` layers never affect input routing.
5. Transition latency from layer open/close signal to routing mode change MUST be <= 100 ms (one render cycle).

## Runtime Telemetry Event Schema (T003)

All telemetry events are emitted to `/api/telemetry/frame` via fire-and-forget POST with `Content-Type: application/json`. Events are batched at the 60-frame boundary or on state change.

```typescript
type TelemetryEventKind =
  | "viewport_start"       // booting → ready: includes fetch_ms + instantiate_ms
  | "first_frame"          // ready → running: includes first_frame_ms from boot
  | "frame_interval"       // steady-state every 60 frames: includes avg_frame_ms
  | "overlay_open"         // overlay layer opened: includes layer name
  | "overlay_close"        // overlay layer closed: includes layer name
  | "degraded_entry"       // entered degraded: includes reason + tick_count
  | "recovery_attempt"     // recovering: includes attempt number
  | "recovery_success"     // recovered: includes total downtime_ms
  | "recovery_failure"     // degraded again: includes attempt number
  | "stopped"              // runtime stopped: includes total_frames + uptime_ms

interface RuntimeTelemetryEvent {
  kind: TelemetryEventKind;
  ts: number;           // Date.now() at emission
  session_id: string;   // stable UUID per component mount
  payload: Record<string, number | string | boolean>;
}
```

## Assumptions

- Existing spec 001 game-engine core remains the underlying engine contract.
- React web remains the first required channel for proving viewport-overlay architecture.
- Electron and mobile channels can follow once React web contract is stable.
- Current API telemetry and game-state endpoints remain available during this migration slice.
- Baseline hardware and browser test matrix will be documented in this feature's plan artifacts.
