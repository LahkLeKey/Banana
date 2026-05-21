# Banana Game World Analysis & Expansion Priorities

**Analyzed**: May 20, 2026 | **Scope**: Native C engine + TypeScript API integration

---

## 1. Current Game World Summary

### World Dimensions & Capacity
| Component | Current | Notes |
|-----------|---------|-------|
| **Terrain Grid** | 18×18 tiles | Island generation via WFC/CA |
| **Terrain Chunks** | 9 (3×3) @ 6×6 each | Chunked for dirty-region rebuild |
| **Entity Capacity** | 2,048 max | Currently ~5 active at init (player + 4 NPCs) |
| **Tick Rate** | 30 TPS | 1000ms / 30 = 33.3ms per frame |
| **Player Input** | 3-way directional | (-1, 0, +1) on X/Z axes only |
| **Multiplayer** | 2-second staleness window | Sync ownership moved to C (recent) |

### Terrain & Biome System
- **7 Tile Types**: WATER_DEEP, WATER_SHALLOW, SAND, GRASS, GROVE, HILL, RIDGE
- **Elevation Layers**: 4 max (height grid: 0–3)
- **Resource System**: Per-tile `resource_bias` field (defined but **unused**)
- **Procedural Generation**: Island algo + WFC post-process + Cellular Automata smoothing
- **Rendering**: Chunked mesh rebuild for dirty regions only

### Current Entity Types & Behaviors

| Entity Type | Current Implementation | Count |
|------------|----------------------|-------|
| **PLAYER** | 1 spawned at origin (0,0), scale 1.25×0.95×1.10 | 1 |
| **NPC** | 2 of 4 initial actors, wildlife controller only | 2 |
| **DYNAMIC** | 2 of 4 initial actors, physics bodies | 2 |
| **STATIC** | None currently | 0 |
| **TRIGGER** | Defined but unused | 0 |

### NPC AI System (Wildlife Controller)
- **State Machine**: 4-state FSM (IDLE → PATROL → INVESTIGATE → RETURN → IDLE)
- **Patrol Behavior**: Square loops around spawn origin (4 waypoints)
- **Perception**: Proximity signal system (`runtime_wildlife_signal_player_nearby`)
- **Navigation**: Pathfinding navigation module (defined but minimal integration)
- **Controller Registry**: Extensible factory pattern (only "wildlife" registered)

### Game Mechanics (Functional)
✓ Player movement with terrain-following  
✓ Multiplayer sync with C-owned staleness detection  
✓ Physics bodies + collider broad-phase  
✓ Raycasting (click targeting de-scoped)  
✓ Signal queue system (entity-to-entity messaging)  
✓ Terrain height sampling for entity placement  

### Game Mechanics (Defined but Dormant)
- Trigger volumes (TRIGGER entity type, no handlers)
- Resource bias per tile (no harvesting/consumption)
- Navigation waypoints (no dynamic pathing)
- Perception system (defined, limited use)
- State machine transitions (only wildlife uses FSM)

---

## 2. Potential Expansion Areas (Ranked by Dependency & Impact)

### **Tier 1: Foundation Enablers** (Required for other features)
These unlock multiple downstream expansions with minimal cross-domain risk.

#### **1a. Interactive Objects Framework**
- **What**: Spawn static/dynamic objects (barrels, chests, trees) that can be interacted with
- **Leverage**: Signal system, entity types (STATIC), world queries
- **Effort**: Low (existing infrastructure)
- **Blocks**: 1b, 1c, 1d, 2a
- **Example**: Barrel at position (5, 0, 5) → player right-click → signal "player_interact_barrel" → barrel state changes
- **Code Entry**: `src/native/engine/runtime/interactive_objects.c` + `src/typescript/api/src/services/interactionService.ts`

#### **1b. NPC Type Registry** (Expand Controller System)
- **What**: Add 2–3 new controller types beyond "wildlife" (merchant, guard, passive animal)
- **Leverage**: Existing controller factory pattern, state machine
- **Effort**: Medium (3–4 new FSM variations)
- **Blocks**: 2a, 2b, 2d
- **Example Types**:
  - `merchant`: IDLE at fixed location, responds to "player_talk" signal
  - `guard`: PATROL on grid path, ALERT on player proximity
  - `fauna` (boar, deer): Grazing behavior, flee on player approach
- **Code Entry**: `src/native/engine/ai/merchant_controller.c`, `guard_controller.c`, `fauna_controller.c`

#### **1c. Terrain Feature Placement**
- **What**: Seed landmark/POI data during world generation (ore deposits, trees, buildings)
- **Leverage**: WFC tile data, resource_bias field, procedural generation
- **Effort**: Medium (tile→feature mapping)
- **Blocks**: 1a, 2a, 2b
- **Example**: Hills → ore deposits, Groves → trees, Grass → buildings
- **Code Entry**: `src/native/engine/runtime/terrain_features.c` (new)

#### **1d. Dynamic Resource Layer**
- **What**: Track consumable resources on terrain (ore, wood, food) as entity-backed state
- **Leverage**: Entity grid, resource_bias, signal system
- **Effort**: Medium (resource mesh + harvesting rules)
- **Blocks**: 2a, 2c, 2d
- **Example**: Wood pile at grove tile → NPC gathers → player harvests
- **Code Entry**: `src/native/engine/runtime/resource_system.c` (new)

---

### **Tier 2: Gameplay Loops** (Player engagement)
Build on Tier 1 to create repeatable player activities.

#### **2a. Collection/Harvesting Loop**
- **What**: Player interacts with resources → gains items → carries in inventory
- **Requires**: 1a (interactive), 1c (terrain features), 1d (resources)
- **Effort**: Medium–High (inventory, weight, UI sync)
- **Example**: Player walks to grove → clicks tree → animates → +5 wood
- **Code Entry**: 
  - `src/typescript/api/src/services/inventoryService.ts`
  - `src/native/engine/runtime/harvest_action.c`
  - React UI: `src/typescript/react/src/components/InventoryPanel.tsx`

#### **2b. Simple Trading/Dialog**
- **What**: Merchant NPCs buy/sell items; player opens dialog, selects items
- **Requires**: 1a (interact), 1b (merchant NPC)
- **Effort**: Medium (dialog state machine, item exchange)
- **Example**: Player right-clicks merchant → dialog lists [wood: 5 → gold: 1] → confirms trade
- **Code Entry**:
  - `src/native/engine/ai/merchant_trading.c`
  - `src/typescript/api/src/routes/trading.ts`
  - React UI: `src/typescript/react/src/components/MerchantDialog.tsx`

#### **2c. Guard/Safety Zone System**
- **What**: Guard NPCs patrol safe zone; hostile creatures spawn outside; player can hide near guards
- **Requires**: 1a (interact), 1b (guard NPC), 1d (resource/spawning)
- **Effort**: Medium (zone logic, threat AI)
- **Example**: Guard patrols village center; boar NPC spawns at woods; player sees threat level UI
- **Code Entry**:
  - `src/native/engine/runtime/zone_system.c`
  - `src/native/engine/ai/threat_detection.c`

#### **2d. NPC Daily Routines**
- **What**: NPCs move between locations on a schedule (home → market → patrol → home)
- **Requires**: 1b (NPC types), 1c (terrain features/POIs)
- **Effort**: Medium (time scheduler, waypoint system)
- **Example**: Merchant at market 6am–6pm, home rest 6pm–6am
- **Code Entry**: `src/native/engine/runtime/npc_scheduler.c` (new)

#### **2e. Simple Quest Markers**
- **What**: NPCs give 1–2 quest types (fetch, explore); player sees markers
- **Requires**: 2a (harvesting), 2b (trading)
- **Effort**: Low–Medium (marker UI, quest state)
- **Example**: Merchant asks for 10 wood → player collects → returns → reward
- **Code Entry**:
  - `src/typescript/api/src/services/questService.ts`
  - React UI: `src/typescript/react/src/components/QuestLog.tsx`

---

### **Tier 3: Depth & Polish** (Long-term engagement)
Add complexity after Tier 1–2 stabilize.

#### **3a. Environmental Hazards & Obstacles**
- **What**: Water blocks, cliffs, lava; NPCs/player navigate around
- **Requires**: Tier 1–2 stable, physics/collider expansion
- **Effort**: High (physics integration, collision rules)
- **Example**: Lava tile ≈ damage on contact; cliff edge ≈ fall damage if jumped

#### **3b. Day/Night Cycle & Weather**
- **What**: Time progression → lighting/behavior changes; optional weather effects
- **Requires**: Tier 2 (NPC routines)
- **Effort**: Medium (time ticker, shader bindings, NPC mood)
- **Example**: 6pm → dusk → NPCs return home, guards alert, mobs spawn

#### **3c. Procedural Dungeons/Interiors**
- **What**: Static building entities teleport player to interior map (separate 18×18 grid)
- **Requires**: Entity teleport, interior terrain, interior mob spawning
- **Effort**: High (state management, interior generation)
- **Example**: Click door → interior cave tile map → combat/loot → exit

#### **3d. Mob Waves & Difficulty Scaling**
- **What**: Periodic hostile spawn waves; difficulty increases with player progress
- **Requires**: Tier 2 (guards, threat system), new aggressive AI
- **Effort**: Medium–High (spawner logic, difficulty curve)

#### **3e. Crafting System**
- **What**: Combine harvested items into new items (2 wood + rope → bow)
- **Requires**: 2a (harvesting), inventory system
- **Effort**: Medium (recipe table, crafting UI, validation)

---

## 3. Recommended Expansion Roadmap

### **Phase 1: Foundation (Weeks 1–2)**
Focus on **Tier 1** to establish patterns and unblock concurrent work.

| Task | Owner | Effort | Blockers | Outcome |
|------|-------|--------|----------|---------|
| **T1.1** Interactive Object Framework | native-wrapper | 2–3d | None | Click barrel → interact signal |
| **T1.2** NPC Type Registry (merchant + 1 fauna type) | native-core | 3–4d | None | 3 controller types registered |
| **T1.3** Terrain Feature Placement | native-core | 2–3d | None | Grove tiles spawn trees |
| **T1.4** Resource Layer (wood/ore tracking) | native-wrapper + API | 2–3d | T1.3 | Resources visible in world |

**Success Criteria**:
- ✓ 5 new native tests passing
- ✓ No FPS regression on 30 TPS
- ✓ Manual test: click tree → gather wood

---

### **Phase 2: First Gameplay Loop (Weeks 3–4)**
Build **Tier 2a–2b**: harvesting + trading.

| Task | Owner | Effort | Blockers | Outcome |
|------|-------|--------|----------|---------|
| **T2.1** Inventory Service (C-backed state) | API interop | 2–3d | T1.4 | Player carries items |
| **T2.2** Harvest Action (tree click → +wood) | native-gameplay | 2–3d | T1.1, T1.4 | Harvesting works end-to-end |
| **T2.3** Merchant Dialog & Trading Logic | native-AI + API | 2–3d | T1.2 | Trade wood → gold |
| **T2.4** React Inventory & Trade UI | React-UI | 2–3d | T2.1, T2.3 | UI shows inventory & dialog |

**Success Criteria**:
- ✓ Manual 2-player test: both harvest, both trade
- ✓ Inventory persists across session
- ✓ Merchant dialog syncs via websocket

---

### **Phase 3: Quest & Routine Foundation (Week 5)**
Enable **Tier 2d–2e**: NPC routines + simple quests.

| Task | Owner | Effort | Blockers | Outcome |
|------|-------|--------|----------|---------|
| **T3.1** NPC Scheduler (time-based waypoint routing) | native-runtime | 2–3d | T1.2, T2.1 | Merchant moves on schedule |
| **T3.2** Quest State Service | API service | 2–3d | T2.3 | Quests track progress |
| **T3.3** Quest UI & Markers | React-UI | 1–2d | T3.2 | Quest log visible, markers shown |

---

### **Phase 4: Safety & Content Depth (Week 6+)**
Stabilize with **Tier 2c** guard/zones, then explore **Tier 3**.

---

## 4. High-Confidence Next Steps

**Immediate (This Week)**:
1. **Code Review**: Gather team feedback on Phase 1 task breakdown
2. **Spike**: Confirm interactive object click detection works in current renderer (20m)
3. **Prototype**: Single merchant NPC patrol in native tests (4h)

**Medium-term (Next 2 Weeks)**:
- Implement **T1.1** + **T1.2** in parallel (non-blocking)
- Validate Tier 1 via integration tests before Tier 2 work starts
- Draft Tier 3 scope once Tier 2 is stable

---

## 5. Risk & Mitigation

| Risk | Impact | Mitigation |
|------|--------|-----------|
| Inventory system bloats multiplayer snapshots | High | Cap inventory to 16 slots; use delta compression |
| Merchant dialog state goes stale across clients | Medium | Validate dialog state on each transaction |
| Resource spawning causes entity cap exhaustion | High | Implement resource pooling; cap total resources at 256 |
| NPC pathfinding causes frame drops | Medium | Implement path caching; batch updates every N ticks |

---

## 6. Confidence Summary

| Dimension | Level | Notes |
|-----------|-------|-------|
| **Architecture Ready** | 85% | Controller system, signal infrastructure solid; inventory & dialog TBD |
| **Native Rendering** | 90% | Mesh/material system mature; interactive feedback TBD |
| **Multiplayer Sync** | 90% | C-owned staleness proven; inventory sync TBD |
| **Timeline Feasibility** | 80% | Tier 1–2 achievable in 4 weeks; Tier 3 scope-dependent |
| **Technical Debt** | Moderate | Resource system underutilized; pathfinding minimally used; no crafting schema yet |

---

**Generated**: 2026-05-20 | **Thoroughness**: Medium-scope analysis covering architecture, mechanics, & prioritized roadmap
