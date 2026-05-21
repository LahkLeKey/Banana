# Specification: Multiplayer with Server-Authoritative Gameplay (Spec 050)

**Version**: 1.2 | **Date**: 2026-05-15 | **Status**: Absorbed by Spec 051 | **Author**: Game Architecture  
**Jurisdiction**: Federal multiplayer authority across server state, prediction, replication, heartbeats, and anti-cheat  
**Agent of Record**: Architecture Council delegating to domain-specific implementation agents  
**Supersedes**: 048-player-controller-foundation, 049-arpg-view-actors  
**Epic**: Multiplayer Foundation — Server-authoritative gameplay with early-stage client-side prediction  

**Baseline Authority**: This spec is preserved as source lineage and evidence context. Active execution authority is now merged into Spec 051.

**Execution Status**: Constitutional docket stages 0-5 completed with recorded heartbeat evidence.

**Execution Continuity Note (2026-05-15)**: Spec 050 multiplayer authority is merged into Spec 051 unified baseline for single-surface execution.

**Post-Docket Note**: TypeScript API anti-cheat interop is currently fulfilled via boundary adapter contract; direct native invocation parity is tracked as a non-blocking follow-up hardening item.

---

## Purpose

Establish a server-authoritative gameplay architecture that ensures:
- **Single Source of Truth**: Game state lives on the server; clients receive authoritative updates
- **Deterministic Simulation**: Native C engine processes actions in sequence; clients reconcile with server
- **Client-Side Prediction**: Optimistic input handling for responsiveness; rollback on server corrections
- **Replication Contract**: Efficient delta updates for multiple concurrent players
- **Platform Parity**: Same code paths for web (WebGL2), mobile (React Native), and desktop (Electron)

This spec defines the network layer, replication protocol, and domain services required for multiplayer while keeping the single-player ARPG foundation intact.

---

## Architecture Overview

### Three-Layer Multiplayer Stack

```
┌────────────────────────────────────────────────────────────────┐
│  Presentation Layer (React/Electron/React Native)             │
│  ├─ Input UI (MobileRadialControl, ContextMenu)               │
│  ├─ State Display (synchronized via ReplicationDomain)       │
│  └─ Error/Status Overlays (SplashOverlay, ErrorBadge)         │
└────────────────────────────────────────────────────────────────┘
                              │
                  GameEngineUIService (Phase 2)
                    ↓ Input Events ↑ State Changes
┌────────────────────────────────────────────────────────────────┐
│  Netcode Layer (TypeScript/Fastify API)                       │
│  ├─ NetcodeDomain: Input buffering, lag compensation         │
│  ├─ ReplicationDomain: Server state ↔ Client sync            │
│  ├─ PredictionDomain: Client-side optimistic simulation      │
│  └─ ConnectionManager: Connection pool, heartbeat, recovery  │
└────────────────────────────────────────────────────────────────┘
                              │
                 WebSocket (with HTTP fallback)
                              │
┌────────────────────────────────────────────────────────────────┐
│  Server Authoritative Core (C/WASM + Fastify)                │
│  ├─ GameState Repository: Authoritative world state          │
│  ├─ ActionDispatcher: Processes input in tick order          │
│  ├─ ReplicationService: Broadcasts delta updates             │
│  ├─ SessionManager: Player connections, identity, timings    │
│  └─ PersistenceLayer: World snapshots for rejoin             │
└────────────────────────────────────────────────────────────────┘
```

### Key Design Principles

1. **Server Authority** — Server is the source of truth; clients are views of server state
2. **Optimistic Input** — Client sends input immediately; predicts outcome locally
3. **Deterministic Tick** — Both server and client run same simulation with same input history
4. **Delta Replication** — Only changed state fields sent to clients (bandwidth efficiency)
5. **Tick-Based** — Server ticks at fixed 30 FPS (33.3ms); clients interpolate between ticks
6. **Session Binding** — Each player has immutable session ID; rejoin rebuilds client from latest snapshot

---

## In Scope *(mandatory)*

- ✅ **[Phase 2]** Integrate GameEngineUIService into GameEnginePage React component
  - Replace direct state refs with service instance
  - Subscribe to state changes for component updates
  - Register action handlers through service APIs
  - All 19 existing tests continue to pass

- **NetcodeDomain** — Client-side input buffering and latency compensation
  - `InputBuffer` entity: Queue input commands with server tick acknowledgment
  - `PredictionTracker` entity: Cache predicted outcomes for rollback
  - Value objects: `InputCommand` (immutable player intent), `TickSnapshot` (predicted state at tick N)
  - Test suite: Input ordering, prediction cache eviction, network lag simulation

- **ReplicationDomain** — Stateful world synchronization
  - `ReplicationService` entity: Track replicated entities (players, actors, terrain)
  - `DeltaUpdate` value object: Only changed fields sent (e.g., `{ entityId, position, rotation }`)
  - `WorldState` value object: Current canonical state from server
  - `MessageCodec` utility: Serialize/deserialize domain objects for wire transfer
  - Test suite: Delta computation, state merging, version conflict resolution

- **PredictionDomain** — Client-side optimistic action processing
  - `LocalSimulator` entity: Run same C engine logic on client (deterministic input → output)
  - `PredictionResult` value object: Predicted state changes before server ACK
  - Rollback strategy: On server contradiction, merge server truth with client uncommitted inputs
  - Test suite: Deterministic output validation, rollback correctness

- **ConnectionManager** — Network lifecycle and recovery
  - Session handshake: Client connects, server assigns player ID + session token
  - Heartbeat: Periodic keep-alive to detect stale connections
  - Reconnection: Rejoin with session ID retrieves latest snapshot + replay buffer
  - Graceful shutdown: Flush pending updates before disconnect

- **Native Anti-Cheat Pipeline (Server-Authoritative)**
  - Native C scoring core handles heartbeat and telemetry ingestion in O(1) per event
  - Usermode telemetry lane: debugger state, code-integrity mismatch count, memory-map anomalies, timing anomalies
  - Suspicious driver lane (ring-1/ring-2 class telemetry): suspicious driver count, unsigned driver count, hidden-module count
  - Multi-layer heartbeats: transport heartbeat, session heartbeat, simulation/integrity heartbeat
  - Policy outputs: risk score, flagged state, rolling integrity hash for reconciliation
  - Future hardening priorities are tracked in anti-cheat-drilldown.md and should remain additive to the current baseline rather than replacing it
  - See drilldown spec: `anti-cheat-drilldown.md`

- **Type Definitions** — Shared protocol types
  - `ServerMessage` (union): `{ type: 'state_update' | 'tick_ack' | 'player_joined' | 'error', ... }`
  - `ClientMessage` (union): `{ type: 'input_command' | 'action_request' | 'ping', ... }`
  - `Actor` (replicated entity): `{ id, type, position, rotation, health, animation, owner }`
  - `Player` (replicated entity): `{ sessionId, name, actor, inputSequence }`

- **API Endpoints** (Fastify runtime contract)
  - `POST /api/game/session/start` — Create player session, return session token + initial world state
  - `WebSocket /api/game/realtime` — Bidirectional stream for input, replication, ticks
  - `POST /api/game/session/rejoin` — Rejoin with session ID, fetch replay buffer + latest snapshot
  - `POST /api/game/session/end` — Graceful disconnect, save player state

- **Absorbed Foundation Integration** — Single-player gameplay remains functional
  - ARPG viewport, camera behavior, context menu all work in single-player mode
  - Server-authority layer is transparent to single-player (local server instance)
  - No breaking changes to existing GameEnginePage contract

---

## Out of Scope *(future slices)*

- Player authentication/authorization (defer to separate identity slice)
- Persistent character progression (defer to persistence layer slice)
- Matchmaking/lobby system (defer to session orchestration)
- Cross-play parity (mobile/desktop networking differences handled separately)
- Large-scale (100+ player) optimization (scale optimization slice)
- Voice/video comms (separate media integration)

---

## Success Criteria

1. **Local Multiplayer Test** — Two browser tabs (same machine) can connect to local server and see each other
2. **Network Resilience** — Client automatically reconnects on connection drop; server restores session state
3. **Latency Hiding** — Input feels responsive even on 100ms+ network delay (prediction + interpolation)
4. **State Consistency** — No desyncs between server and multiple clients over 5-minute session
5. **Single-Player Compat** — Absorbed ARPG foundation scenes work unchanged with server-authority layer
6. **Determinism Proof** — Replay same input sequence on client and server produces identical output
7. **Zero Rebase** — No changes required to GameEnginePage, InputDomain, InteractionDomain, ViewportDomain
8. **Protocol Efficiency** — Delta updates ≤ 20% of full state size; tick rate ≥ 30 FPS at 100ms latency

---

## Domain Entities & Value Objects

### NetcodeDomain

**InputBuffer (Entity)**
- Methods: `enqueue(command)`, `dequeue()`, `get(tickIndex)`, `clear()`
- State: Ordered queue of `InputCommand`; tracks server acknowledgment per tick
- Invariant: Commands never reordered; gaps filled with neutral input

**InputCommand (Value Object)**
```typescript
{
  playerSessionId: string;
  inputSequence: number;          // Monotonic counter
  tickIndex: number;              // Server tick when applied
  moveX: -1 | 0 | 1;
  moveZ: -1 | 0 | 1;
  actionId?: string;              // "interact" | "harvest" | null
  targetNormalizedX?: number;     // [0, 1] normalized coordinate
  targetNormalizedY?: number;
  timestamp: number;              // Client time (for lag estimation)
}
```

**PredictionTracker (Entity)**
- Methods: `cache(tickIndex, state)`, `get(tickIndex)`, `rollback(tickIndex)`
- Keeps last 60 ticks of predicted state (2 seconds at 30 FPS)
- On server ACK: Validate mismatch; if incorrect, trigger rollback + replay

**TickSnapshot (Value Object)**
```typescript
{
  tickIndex: number;
  entities: Map<entityId, EntitySnapshot>;
  timestamp: number;
  isAuthoritative: boolean;       // false = prediction, true = server
}
```

### ReplicationDomain

**ReplicationService (Entity)**
- Methods: `subscribe(entityId, listener)`, `applyDelta(delta)`, `getWorldState()`, `serialize()`
- Maintains shadow copy of server world state
- Notifies listeners of entity changes (position, rotation, health, etc.)

**DeltaUpdate (Value Object)**
```typescript
{
  entityId: string;
  tick: number;
  changes: Record<string, unknown>;  // Only fields that changed
  timestamp: number;
}
```

**WorldState (Value Object)**
```typescript
{
  tick: number;
  timestamp: number;
  players: Record<sessionId, Player>;
  actors: Record<actorId, Actor>;
  terrain?: Record<chunkId, TerrainData>;  // Future
  isAuthoritative: boolean;
}
```

**MessageCodec (Utility)**
- `encode(object)`: Serialize to compact wire format (ArrayBuffer or msgpack)
- `decode(buffer)`: Deserialize back to domain objects
- Handles cross-version compatibility

### PredictionDomain

**LocalSimulator (Entity)**
- Methods: `step(input)`, `setState(worldState)`, `getState()`
- Runs deterministic engine simulation on client
- Uses same tick logic as server (via WASM module)

**PredictionResult (Value Object)**
```typescript
{
  tickIndex: number;
  inputApplied: InputCommand;
  stateAfter: TickSnapshot;
  confidence: number;             // 1.0 = matches server, < 1.0 = mismatch
  divergenceReason?: string;      // "physics", "action_result", "actor_state"
}
```

---

## Integration with Existing Architecture

### No Breaking Changes

- **GameEngineUIService** — Unchanged (Phase 2 integration proceeds in parallel)
- **InputDomain** — Unchanged; NetcodeDomain sits above it (input buffer wraps InputAggregator)
- **InteractionDomain** — Unchanged; PredictionDomain triggers action handlers same way
- **ViewportDomain** — Unchanged; clients calculate viewport independently
- **MobileRadialControl, ContextMenu, Overlays** — Unchanged; presentation layer agnostic to network

### New Dependencies

- **GameEnginePage** imports `ConnectionManager` to establish session on mount
- **GameEnginePage** subscribes to `ReplicationService` state changes instead of just local state
- **Event handlers** delegate input to `InputBuffer` before local simulation
- **Action handlers** flow through `PredictionDomain` for optimistic execution

---

## Protocol Layer

### Server → Client (WebSocket message schema)

```json
{
  "type": "tick_advance",
  "tick": 120,
  "timestamp": 1715795400000,
  "deltas": [
    {"entityId": "player_1", "changes": {"position": [10.5, 5.0], "animation": "walk_n"}},
    {"entityId": "actor_2", "changes": {"health": 45}}
  ]
}
```

### Client → Server (WebSocket message schema)

```json
{
  "type": "input_command",
  "sequence": 340,
  "tick": 120,
  "moveX": 1,
  "moveZ": 0,
  "actionId": null,
  "timestamp": 1715795400050
}
```

---

## Testing Strategy

1. **Unit Tests** — NetcodeDomain, ReplicationDomain, PredictionDomain (all Bun test suite)
2. **Integration Tests** — InputDomain ↔ NetcodeDomain; InteractionDomain ↔ PredictionDomain
3. **Network Simulation** — Simulate latency, packet loss, reordering in test harness
4. **Determinism Tests** — Replay same input sequence; verify server and client outputs match
5. **E2E Tests** — Two browser instances connect to local server; validate sync over 2min session
6. **Regression** — All 19 GameEnginePage tests continue to pass
7. **Native Anti-Cheat Tests** — Validate monotonic heartbeat detection, suspicious driver scoring, and integrity hash progression

---

## Performance Requirements

| Metric | Target | Rationale |
|--------|--------|-----------|
| **Tick Rate** | 30 FPS (33.3ms) | CPU-bound server-side physics |
| **Input Latency** | ≤ 60ms (w/ prediction) | 2 ticks at 30 FPS; imperceptible to player |
| **Replication Bandwidth** | ≤ 50 Kbps per player | 30 FPS × ~200 bytes/update |
| **Prediction Accuracy** | ≥ 95% match rate | Server corrections ≤ 5% of commands |
| **Reconnect Time** | ≤ 5 seconds | Session restore + state rebuild |
| **Memory (Client)** | ≤ 50 MB for 10 players | Prediction cache + entity replicas |

---

## Implementation Timeline

**Phase 1 (Week 1-2)**: Domain design + type contracts  
**Phase 2 (Week 2-3)**: GameEngineUIService integration + basic netcode wiring  
**Phase 3 (Week 3-4)**: Full replication + prediction implementation  
**Phase 4 (Week 4-5)**: End-to-end testing + optimization  
**Phase 5 (Week 5-6)**: Failover + session management  

---

## Acceptance Criteria (Per Task)

- ✅ NetcodeDomain fully typed and exported from @banana/ui
- ✅ ReplicationDomain fully typed and exported from @banana/ui
- ✅ PredictionDomain fully typed and exported from @banana/ui
- ✅ ConnectionManager wired into GameEnginePage (Phase 2)
- ✅ Local multiplayer test passes (two browser tabs sync state)
- ✅ All 19 GameEnginePage tests continue passing
- ✅ All new domain tests (NetcodeDomain, ReplicationDomain, PredictionDomain) passing
- ✅ Zero breaking changes to existing component/domain contracts

---

## References

- Spec lineage archive entries for 048 and 049 under `.specify/legacy-baseline/specs-archive/2026-05-multiplayer-baseline-reset/`
- [Anti-Cheat Drilldown](./anti-cheat-drilldown.md) — Native usermode and suspicious-driver telemetry model
- Game Netcode Resources:
  - "Fast-Paced Multiplayer" (GafferOnGames) — client-side prediction fundamentals
  - "Deterministic Simulation" (INetCode white papers) — tick-based reconciliation
  - "QuakeWorld Networking" — delta compression baseline
