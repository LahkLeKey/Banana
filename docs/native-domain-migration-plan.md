# Native Banana Domain Migration Plan

## Scope

This plan keeps the native core organized around banana domain contexts while removing the last legacy compatibility file names from the repo. The shared entry point is now `src/native/core/main.h` and `src/native/core/main.c`, with all domain-owned implementations living under `src/native/core/domain`.

## Domain Layout

### Core Main

- `src/native/core/main.h` re-exports the native banana domains for callers that still need one core entry point.
- `src/native/core/main.c` owns shared identifier helpers and banana naming helpers.

### Legacy Projection Compatibility

- Owns the purchases and multiplier projection path that the wrapper and DAL still call.
- Lives in `src/native/core/domain/banana_projection_legacy.h` and `src/native/core/domain/banana_projection_legacy.c`.
- Exists only to preserve current interop behavior while the wrapper contract is redesigned around real banana requests.

### Banana Lifecycle

- Owns cultivar names, plant identity, bunch biology, and ripeness prediction.
- Lives in `src/native/core/domain/banana_lifecycle.h` and `src/native/core/domain/banana_ripeness.c`.

### Cultivation

- Owns plant registration, sucker growth, bunch harvest, bunch ripeness progression, and spoilage transitions.
- Lives in `src/native/core/domain/banana_cultivation.h` and `src/native/core/domain/banana_cultivation.c`.

### Processing

- Owns individual banana entities inside bunch aggregates, bunch factory rules, and harvest-time invariant validation.
- Lives in `src/native/core/domain/banana_processing.h` and `src/native/core/domain/banana_processing.c`.

### Domain Events

- Owns the native event vocabulary for planted, bloomed, harvested, shipped, arrived, ripened, sold, spoiled, and inventory-received flows.
- Lives in `src/native/core/domain/banana_events.h` and `src/native/core/domain/banana_events.c`.

### Supply Chain

- Owns batch registration, batch composition, export transitions, shipment state, distribution node state, and batch-level ripeness lookups.
- Lives in `src/native/core/domain/banana_supply_chain.h` and `src/native/core/domain/banana_supply_chain.c`.

### Inventory

- Owns retail inventory receipt, sell-through, spoilage discard, and reorder signaling.
- Lives in `src/native/core/domain/banana_inventory.h` and `src/native/core/domain/banana_inventory.c`.

### Repositories

- Owns in-memory repository interfaces for plants, bunch aggregates, shipments, and inventory items.
- Lives in `src/native/core/domain/banana_repositories.h` and `src/native/core/domain/banana_repositories.c`.

### Domain Services

- Owns ripening orchestration for bunch aggregates and quality-control calculations such as average fruit weight and underweight detection.
- Lives in `src/native/core/domain/banana_services.h` and `src/native/core/domain/banana_services.c`.

### Application Services

- Owns command-style orchestration for harvest, ripen, ship, receive, sell, and spoilage-discard workflows against the repositories.
- Lives in `src/native/core/domain/banana_application.h` and `src/native/core/domain/banana_application.c`.

### Integration Adapters

- Owns anti-corruption helpers for external telemetry normalization and external retail inventory translation.
- Lives in `src/native/core/domain/banana_integration.h` and `src/native/core/domain/banana_integration.c`.

### Read Models

- Owns CQRS-style stock views, ripeness reports, and cultivar statistics projections.
- Lives in `src/native/core/domain/banana_read_models.h` and `src/native/core/domain/banana_read_models.c`.

## Migration Rules

1. Put all new native logic in `src/native/core/domain` unless it is a true shared core entry-point concern.
2. Use `src/native/core/main.h` only as the transitional umbrella for callers that still span multiple banana domains.
3. Keep wrapper ABI changes explicit and coordinated; naming cleanup alone should not change the interop contract.
4. Remove legacy projection once wrapper and managed callers stop depending on purchases and multiplier inputs.

## Incremental Replacement Path

### Phase 1: Remove Legacy Compatibility Naming

- Delete the old compatibility-named core files.
- Replace them with domain-owned source files and `core/main.*`.
- Keep native behavior stable under the existing tests.

### Phase 2: Narrow Consumer Includes

- Move wrapper and DAL includes to the narrowest domain headers they actually need.
- Leave `main.h` only for callers that legitimately touch multiple domains.

### Phase 3: Replace Legacy Projection Contracts

- Introduce banana-native request models at the wrapper boundary.
- Keep the legacy projection context as an anti-corruption layer until no external caller depends on it.

### Phase 4: Deepen Aggregates

- Add plant, bunch, and shipment invariants directly inside lifecycle and supply-chain code.
- Replace the in-memory batch registry with persistence-backed recovery.

## Current Native Coverage

- Cultivation aggregates: plant registration, sucker propagation, bunch harvest, ripeness progression, and spoilage.
- Harvesting and processing aggregates: individual banana entities, bunch factory creation rules, cultivar invariants, ripeness uniformity checks, and aggregate weight validation.
- Supply chain aggregates: batch registration, batch-to-bunch composition, export transitions, shipment dispatch and arrival, and node inventory movement.
- Retail inventory aggregates: inventory receipt, sale, spoilage discard, and reorder thresholds.
- Domain events: native event payloads now capture planted, bloomed, harvested, shipped, arrived, ripened, sold, spoiled, and inventory-received actions.
- Repositories: native in-memory repository interfaces now exist for plants, bunches, shipments, and inventory.
- Domain and application services: native command handlers now orchestrate harvest, ripen, ship, receive, sell, and discard workflows through repository-backed operations.
- Anti-corruption and CQRS support: native adapters translate external telemetry and inventory records, and native read models project stock, ripeness, and cultivar statistics.

## Validation Matrix

### Native-only Changes

- Build: `Build Native Library`
- Test: `ctest --test-dir build/native -C Release --output-on-failure`
- Optional coverage gate: `bash scripts/run-native-c-tests-with-coverage.sh`

### Cross-layer Follow-up

- Build: `Build Banana API`
- Unit tests: `dotnet test tests/unit/Banana.UnitTests.csproj -c Debug`
- Integration tests: `dotnet test tests/integration/Banana.IntegrationTests.csproj -c Debug`

## Exit Criteria

- No legacy compatibility file names remain in the repo.
- Native core code lives either in `src/native/core/domain` or `src/native/core/main.*`.
- Wrapper ABI remains stable while the domain model keeps replacing the legacy projection path.
- The Banana DDD feature surface described in `docs/banana-ddd.md` is covered inside native core, except for the explicitly out-of-scope consumer context.