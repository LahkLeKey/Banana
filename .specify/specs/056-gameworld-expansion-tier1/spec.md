# Feature Specification: Gameworld Expansion - Tier 1 Foundation

**Feature Branch**: `[056-gameworld-expansion-tier1]`
**Created**: 2026-05-20
**Status**: Draft
**Jurisdiction**: package
**Agent of Record**: native core + API interop + React UI
**Supersedes**: none
**Input**: User direction: "Start expanding the game world" - move from infrastructure refactoring to playable gameplay features

## In Scope *(mandatory)*

- Implement **interactive objects framework** (click detection, state persistence, multi-player sync)
- Extend **NPC type registry** to support merchant archetype in addition to wildlife
- Expand **terrain features** (grove ↦ trees, hills ↦ ore deposits as visual/interactive markers)
- Add **resource layer** (wood/ore as world-state that can be collected and tracked)
- Enable **first repeatable gameplay loop** (click tree → collect wood → talk to merchant)

## Authority and Lineage *(mandatory)*

- **Bounded Context**: `src/native/engine/runtime` entity system + `src/typescript/api/src/routes/game-session.ts` sync + React UI interaction layer
- **Executing Authority**: Native engine + API interop owners, coordinated with React UI for click/interaction feedback
- **Supersession Rule**: Extends 055-c-runtime-parallel-foundation runtime contracts; does not modify parallel policies or tick phases

## Out of Scope *(mandatory)*

- Inventory UI or persistent save/load (deferred to Tier 2)
- NPC daily routines or complex AI (Tier 3)
- Environmental hazards, dungeons, or procedural content beyond terrain features (Tier 3)
- Trading UI or marketplace mechanics (Tier 2)

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Interactive Objects Foundation (Priority: P1)
**Description**: As a player, I can click on interactive objects (trees, barrels) in the game world to trigger actions (collect resources, trigger events).

**Acceptance Criteria**:
- Interactive objects have click-detection boundaries and state (e.g., harvestable, active)
- Clicking an interactive object invokes a resource callback (e.g., +5 wood)
- State changes are synchronized across multiplayer clients
- Click interactions work on both primary player and remote players' clients

**Testing Approach**:
- Unit test: Object registration, click boundary detection, state callbacks
- Integration test: Multiplayer click sync (two clients, verify state consistency)
- Manual: Click tree in React UI, see animation/feedback, verify wood count increases

**Implementation Notes**:
- C-side: New `interactive_object` entity type with boundaries + callback registry
- TypeScript: WebSocket event for click input (client → server)
- React: Click handler that sends click position, receives state update

---

### User Story 2 - NPC Type Registry Expansion (Priority: P2)
**Description**: As a designer, I can define multiple NPC archetypes (merchant, livestock guardian, scout) with different behaviors and interactions.

**Acceptance Criteria**:
- NPC registry supports merchant archetype with buy/sell callbacks
- Wildlife and merchant NPCs coexist in the same world
- NPC types have distinct visual representations (model swap or color/size variation)
- Multiplayer clients see consistent NPC types and behaviors

**Testing Approach**:
- Unit test: NPC type registration, behavior callback dispatch
- Integration test: Spawn merchant + wildlife, verify different signals/movement patterns
- Manual: Visit merchant NPC, see different interaction prompt than wildlife

**Implementation Notes**:
- C-side: Expand controller registry to support merchant signal handlers
- TypeScript: Route NPC interaction to correct handler (trade vs. perception update)
- React: Conditional UI (trade menu for merchant, stat display for wildlife)

---

### User Story 3 - Terrain Feature Expansion (Priority: P2)
**Description**: As a player, I can see and interact with terrain-specific features (trees on grass/grove, ore deposits on hills/ridges).

**Acceptance Criteria**:
- Biome → feature mapping (grove→trees, hill→ore, ridge→ore)
- Features are placed deterministically based on terrain seed (reproducible world)
- Features are clickable (trigger collection action on trees)
- Feature visibility is synchronized across multiplayer clients

**Testing Approach**:
- Unit test: Biome-to-feature mapping, deterministic placement
- Integration test: Feature sync on chunk rebuild or client join
- Manual: Inspect generated world, verify grove has trees and hills have ore

**Implementation Notes**:
- C-side: Extend terrain generation to place feature entities based on tile type
- TypeScript: Route feature click to resource collection callback
- React: Render feature sprites/models over terrain tiles

---

### User Story 4 - Resource Layer (Priority: P2)
**Description**: As a player, I can gather resources from the world and track them (wood from trees, ore from deposits).

**Acceptance Criteria**:
- Wood/ore counts are stored in C-backed world state (not just client-side)
- Collection updates are synchronized across all connected clients
- Resource counts persist within a session and show in UI
- Multiple players can collect from the same resource pile (with limits)

**Testing Approach**:
- Unit test: Resource increment/decrement logic, state persistence
- Integration test: Two clients collect from same object, verify both see updated count
- Manual: Multiple players gather same tree, both see wood count increase

**Implementation Notes**:
- C-side: Extend `NativePlayerBinding` to include inventory (wood/ore counts)
- TypeScript: Sync inventory as part of player state payload
- React: Display current wood/ore in UI stats panel

---

## Definition of Done *(mandatory)*

- [ ] All 4 user stories have passing unit tests (C-side with CTest + TypeScript Jest/similar)
- [ ] Integration tests confirm multiplayer sync of interactive objects and resources
- [ ] Manual testing with 2 clients validates click interactions and state consistency
- [ ] Heartbeat log updated with implementation evidence
- [ ] No regressions: Native tests 27/27 passing, API smoke test succeeds
- [ ] Code follows existing DDD/SOLID patterns (no monolithic modules)

## Technical Approach *(context for reviewer)*

### C-Side Changes
1. **Entity Extension**: Add `interactive_object` entity type with click-boundary + resource callback
2. **NPC Registry**: Expand controller types to support merchant signal patterns
3. **Terrain Features**: Generate feature entities during terrain build, bound to tile biome
4. **Resource State**: Extend player binding to include wood/ore inventory

### TypeScript Bridge
1. **Interaction Events**: New WebSocket message type for click input (position + object ID)
2. **State Sync**: Include interactive object state changes in player snapshot payload
3. **NPC Signals**: Route merchant interactions to trade callback instead of perception

### React UI
1. **Click Handler**: Convert canvas click → position, query server for object under cursor
2. **Resource Display**: Show wood/ore counts in UI stats panel
3. **Feedback**: Animate resource collection (particle effect, counter flash)

### Testing Strategy
- **Unit**: Boundary detection, callback dispatch, state mutations
- **Integration**: Cross-client state sync on object clicks and resource collection
- **Manual**: E2E validation with two-client setup, visual feedback confirmation

---

## Risk Mitigation

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|-----------|
| Click detection latency on high-entity worlds | Medium | Medium | Spatial partitioning (quadtree) in click handler |
| Resource sync race conditions (two clients click same object) | Low | High | Atomic increment with server-side validation |
| Merchant NPC pathfinding to trade location | Low | Medium | Use existing controller attach + waypoint system |
| Inventory desync across clients | Medium | High | Include inventory hash in snapshot, validate on mismatch |

---

## Metrics & Success Criteria

- **Playability**: 2 clients can join, move, click resources, see state sync ✓
- **Performance**: Click response < 100ms, resource sync < 33ms (one frame at 30 TPS)
- **Coverage**: ≥80% code coverage on new interactive object + resource modules
- **Regression**: 0 new test failures, all 27 native tests still passing

