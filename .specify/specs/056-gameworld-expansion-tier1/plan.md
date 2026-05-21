# Implementation Plan: Gameworld Expansion - Tier 1 Foundation

**Date**: 2026-05-20
**Status**: Ready for Design Review

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                    React UI (click handler)                 │
│              (position → server, display feedback)           │
└─────────────────────────────────────────────────────────────┘
                           ↕
┌─────────────────────────────────────────────────────────────┐
│          TypeScript API (game-session.ts)                   │
│  (click event routing, inventory state, snapshot sync)      │
└─────────────────────────────────────────────────────────────┘
                           ↕
┌─────────────────────────────────────────────────────────────┐
│                   Native C Runtime                          │
│  (interactive_object entity, resource collection logic,     │
│   merchant NPC controller, terrain feature placement)       │
└─────────────────────────────────────────────────────────────┘
```

## Module Breakdown

### **Tier 1A: Interactive Object Foundation (Days 1–2)**

**File**: `src/native/engine/runtime/interactive_objects.c` + `.h`
- Purpose: Entity type for clickable objects (trees, barrels)
- Responsibilities:
  - Store object position, click-boundary (radius or AABB)
  - Track object state (active, depleted, cooldown)
  - Dispatch click callback (collect wood, trigger event)
- API:
  ```c
  typedef struct {
    int id;
    float x, y, z;
    float click_radius;
    int state;  // 0=depleted, 1=active, 2=cooldown
    int (*on_click)(const char *player_guid, int object_id);
  } InteractiveObject;

  void interactive_objects_register(const char *object_type, int (*callback)(...));
  int interactive_objects_click(float world_x, float world_z, const char *player_guid);
  void interactive_objects_tick(int64_t dt_ms);
  ```
- Tests: `tests/native/interactive_objects_test.c`

---

### **Tier 1B: Merchant NPC Type (Days 2–3)**

**File**: `src/native/engine/ai/npc_merchant.c` + `.h`
- Purpose: NPC controller for merchant behavior
- Responsibilities:
  - Merchant stands at market location
  - Responds to player proximity with trade signals
  - Track inventory state (dynamically adjust prices)
- Extends: Existing controller registry in `runtime/controller_runtime.c`
- API:
  ```c
  typedef struct {
    int merchant_id;
    float market_x, market_z;
    int wood_inventory;
    int ore_inventory;
    int (*on_trade_request)(int merchant_id, const char *player_guid, ...);
  } MerchantNPC;

  void npc_merchant_register(int merchant_id, float market_x, float market_z);
  void npc_merchant_tick(int merchant_id, int64_t dt_ms);
  ```
- Tests: `tests/native/npc_merchant_test.c`

---

### **Tier 1C: Terrain Feature Placement (Days 3–4)**

**File**: `src/native/engine/runtime/terrain_features.c` + `.h`
- Purpose: Place interactive objects on terrain based on biome
- Responsibilities:
  - Define biome → feature mappings (grove → trees, hill → ore)
  - Place features deterministically during terrain generation
  - Register feature entities in interactive object registry
- Integration Point: Called from `runtime/terrain_generation.c` during chunk generation
- API:
  ```c
  void terrain_features_place_on_chunk(
    int chunk_x, int chunk_z,
    const uint8_t *tile_types,    // 9x9 array of BANANA_TILE_*
    InteractiveObject *out_objects,
    int *out_count
  );
  ```
- Tests: `tests/native/terrain_features_test.c`

---

### **Tier 1D: Resource Inventory State (Days 4–5)**

**File**: Extend `src/native/engine/runtime/player_registry.c`
- Purpose: Track player resource inventory (wood, ore)
- Changes:
  - Add to `NativePlayerBinding` struct:
    ```c
    int wood_count;
    int ore_count;
    ```
  - Add increment function:
    ```c
    void runtime_player_registry_add_resource(
      const char *player_guid,
      const char *resource_type,  // "wood" or "ore"
      int amount
    );
    ```
- Tests: Extend `tests/native/player_sync_staleness_test.c` with inventory subset

---

### **Tier 1E: TypeScript Bridge (Days 5–6)**

**File**: `src/typescript/api/src/routes/game-session.ts`
- New Event Handler:
  ```typescript
  interface GameClickCommandEnvelope {
    readonly sequence: number;
    readonly tick: number;
    readonly clickX: number;
    readonly clickZ: number;
    readonly timestamp: number;
  }

  function handleClickInput(event, session) {
    // 1. Call C: engine_world_click_at(clickX, clickZ, playerId)
    // 2. Update player inventory in response
    // 3. Broadcast updated snapshot to all clients
  }
  ```
- Snapshot Extension:
  - Include player inventory (wood/ore) in `NativeEngineSnapshot`
  - Include interactive object state in entity records

---

### **Tier 1F: React UI Interaction (Days 6–7)**

**File**: `src/typescript/react/src/components/GameCanvas.tsx` (or equivalent)
- New Features:
  - Canvas click handler that converts screen coords → world coords
  - Send `GameClickCommandEnvelope` to server
  - Display resource collection feedback (particle effect, counter flash)
  - Render inventory UI (wood/ore counts)
- Tests: Jest snapshot tests for UI feedback logic

---

## Implementation Sequence

### **Phase 1: C-Side Foundation (Days 1–4)**
1. **Day 1–2**: `interactive_objects.c` + tests (click detection, callback dispatch)
2. **Day 2–3**: `npc_merchant.c` + tests (merchant NPC behavior)
3. **Day 3–4**: `terrain_features.c` + tests (biome → feature placement)
4. **Day 4–5**: Extend `player_registry.c` with inventory (inventory sync)

**Validation**: All new modules compile, 30+ C tests passing

### **Phase 2: TypeScript Bridge (Days 5–6)**
5. **Day 5–6**: Extend `game-session.ts` with click handler + inventory sync

**Validation**: API smoke test passes, no TypeScript errors

### **Phase 3: React UI (Days 6–7)**
6. **Day 6–7**: Add canvas click handler + inventory display in React

**Validation**: Manual two-client test: click tree → both see wood +5

---

## Dependency Graph

```
terrain_features.c
    ↓
    └─→ interactive_objects.c
             ↓
             └─→ game-session.ts (click handler)
                  ↓
                  └─→ GameCanvas.tsx (UI)

npc_merchant.c
    ↓
    └─→ controller_runtime.c (already exists)
         ↓
         └─→ game-session.ts (NPC sync)
              ↓
              └─→ GameCanvas.tsx (NPC display)

player_registry.c (extend existing)
    ↓
    └─→ game-session.ts (inventory snapshot)
         ↓
         └─→ GameCanvas.tsx (inventory UI)
```

## Data Model Changes

### C-Side World State

```c
// New: interactive_object registry
typedef struct {
  int id;
  int type;           // INTERACTIVE_TYPE_TREE, etc.
  float x, y, z;
  float click_radius;
  int state;          // ACTIVE, DEPLETED, COOLDOWN
  int resource_yield; // 5 wood, 3 ore, etc.
} InteractiveObject;

// Extend: NativePlayerBinding
typedef struct {
  // ... existing fields ...
  int wood_count;
  int ore_count;
  int64_t last_collection_at;  // cooldown tracking
} NativePlayerBinding;

// New: MerchantNPC
typedef struct {
  int controller_id;
  float market_x, market_z;
  int wood_in_stock;
  int ore_in_stock;
} MerchantNPC;
```

### TypeScript Snapshot Extension

```typescript
interface NativeEngineSnapshot {
  readonly tick: number;
  readonly timestamp: number;
  readonly isAuthoritative: true;
  readonly entities: Record<string, NativeEngineSnapshotEntity>;
  readonly playerInventory?: {
    readonly wood: number;
    readonly ore: number;
  };
}
```

---

## Testing Strategy

### Unit Tests (C-Side)
- **interactive_objects_test.c**: Click detection, state transitions, callback dispatch
- **npc_merchant_test.c**: Merchant pathfinding, trade signal generation
- **terrain_features_test.c**: Biome → feature mapping, deterministic placement
- **player_registry.c** (extend): Inventory increment, resource limits

### Integration Tests (C ↔ TypeScript)
- **game-session.test.ts**: Click event routing, inventory sync, multiplayer consistency
- Mock native engine to simulate click responses

### E2E Tests (Manual)
- Two clients in local compose environment
- Client A clicks tree → both see wood +5
- Inventory persists across reconnection (within session)

### Coverage Gate
- Minimum 80% line coverage on new modules
- All 27 existing tests must still pass

---

## Risk Mitigation

| Risk | Strategy |
|------|----------|
| Click detection performance on high-entity worlds | Use spatial quadtree for click raycast, lazy initialization |
| Resource collection race conditions | Server-side atomic increment with player-specific cooldown |
| Merchant NPC getting stuck | Use existing waypoint/signal system, fail gracefully to IDLE |
| Inventory desync | Include checksum in snapshot, hard-fail and reconnect on mismatch |
| Terrain feature placement non-determinism | Derive feature placement from terrain seed, validate in tests |

---

## Deliverables

- **Code**: 6 new C modules + 1 TypeScript route update + React UI enhancement
- **Tests**: 30+ new C unit tests, 10+ TypeScript integration tests
- **Docs**: Heartbeat log with implementation evidence
- **Validation**: Two-client manual test confirming click → collection → sync

---

## Success Criteria

✅ By end of Tier 1:
- Players can click trees and collect wood
- Merchants appear in world and display trade signals
- Inventory state is C-backed and synced across clients
- All 27 existing tests still pass
- Confidence: 80%+ on Tier 2 (trading, quest hooks)

