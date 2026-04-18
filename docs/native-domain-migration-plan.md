# Native Banana Domain Migration Plan

## Scope

This plan keeps the native core organized around banana domain contexts while removing compatibility-era catch-all entry points from the repo. Native behavior now lives in explicit domain-owned files under `src/native/core/domain`, with DAL code in `src/native/core/dal` and wrapper callers including only the domain headers they actually use.

## Domain Layout

### Legacy Projection Compatibility

- Owns the purchases and multiplier projection path that the wrapper and DAL still call.
- Lives in `src/native/core/domain/banana_projection_legacy.h` and `src/native/core/domain/banana_projection_legacy.c`.
- Exists only to preserve current interop behavior while the wrapper contract is redesigned around real banana requests.

### Banana Lifecycle

- Owns cultivar names, plant identity, identifier helpers, and ripeness prediction.
- Lives in `src/native/core/domain/banana_lifecycle.h`, `src/native/core/domain/banana_lifecycle.c`, and `src/native/core/domain/banana_ripeness.c`.

### Cultivation

- Owns plant registration, sucker growth, bunch harvest, bunch ripeness progression, and spoilage transitions.
- Lives in `src/native/core/domain/banana_cultivation.h` and `src/native/core/domain/banana_cultivation.c`.

### Agriculture

- Owns farms, fields, soil conditions, seedlings, and planting or harvest planning helpers.
- Lives in `src/native/core/domain/banana_agriculture.h` and `src/native/core/domain/banana_agriculture.c`.

### Processing

- Owns individual banana entities inside bunch aggregates, bunch factory rules, and harvest-time invariant validation.
- Lives in `src/native/core/domain/banana_processing.h` and `src/native/core/domain/banana_processing.c`.

### Domain Events

- Owns the native event vocabulary for planted, bloomed, harvested, shipped, arrived, ripened, sold, spoiled, and inventory-received flows.
- Lives in `src/native/core/domain/banana_events.h` and `src/native/core/domain/banana_events.c`.

### Supply Chain

- Owns batch registration, batch composition, export transitions, shipment state, distribution node state, and batch-level ripeness lookups.
- Lives in `src/native/core/domain/banana_supply_chain.h` and `src/native/core/domain/banana_supply_chain.c`.

### Logistics

- Owns containers, ripening rooms, temperature settings, and current-location modeling around batch movement.
- Lives in `src/native/core/domain/banana_logistics.h` and `src/native/core/domain/banana_logistics.c`.

### Inventory

- Owns retail inventory receipt, sell-through, spoilage discard, and reorder signaling.
- Lives in `src/native/core/domain/banana_inventory.h` and `src/native/core/domain/banana_inventory.c`.

### Retail

- Owns store sections, barcodes, prices, and retail order fulfillment on top of inventory.
- Lives in `src/native/core/domain/banana_retail.h` and `src/native/core/domain/banana_retail.c`.

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

- Owns anti-corruption helpers for external telemetry normalization, external retail inventory translation, and weather-to-cultivation advice.
- Lives in `src/native/core/domain/banana_integration.h` and `src/native/core/domain/banana_integration.c`.

### Read Models

- Owns CQRS-style stock views, ripeness reports, and cultivar statistics projections.
- Lives in `src/native/core/domain/banana_read_models.h` and `src/native/core/domain/banana_read_models.c`.

## Migration Rules

1. Put all new native logic in `src/native/core/domain` unless it is a true shared core entry-point concern.
2. Keep callers on the narrowest domain headers they need instead of reintroducing umbrella includes.
3. Keep wrapper ABI changes explicit and coordinated; naming cleanup alone should not change the interop contract.
4. Remove legacy projection once wrapper and managed callers stop depending on purchases and multiplier inputs.

## Incremental Replacement Path

### Phase 1: Remove Legacy Compatibility Naming

- Delete the old compatibility-named core files.
- Replace them with domain-owned source files.
- Keep native behavior stable under the existing tests.

### Phase 2: Narrow Consumer Includes

- Move wrapper and DAL includes to the narrowest domain headers they actually need.
- Do not add new umbrella headers back into the native core.

### Phase 3: Replace Legacy Projection Contracts

- Introduce banana-native request models at the wrapper boundary.
- Keep the legacy projection context as an anti-corruption layer until no external caller depends on it.

### Phase 4: Deepen Aggregates

- Add plant, bunch, and shipment invariants directly inside lifecycle and supply-chain code.
- Replace the in-memory batch registry with persistence-backed recovery.

## Current Native Coverage

- Agriculture aggregates: farm and field registration, soil validation, seedling transplants, and planting or harvest planning.
- Cultivation aggregates: plant registration, sucker propagation, bunch harvest, ripeness progression, and spoilage.
- Harvesting and processing aggregates: individual banana entities, bunch factory creation rules, cultivar invariants, ripeness uniformity checks, aggregate weight validation, crate packing, and inspection records.
- Supply chain aggregates: batch registration, batch-to-bunch composition, export transitions, shipment dispatch and arrival, and node inventory movement.
- Logistics aggregates: containers, temperature settings, ripening rooms, and room-driven ripeness projections.
- Retail inventory aggregates: inventory receipt, sale, spoilage discard, reorder thresholds, store sections, pricing, barcodes, and retail order fulfillment.
- Domain events: native event payloads now capture planted, bloomed, harvested, shipped, arrived, ripened, sold, spoiled, and inventory-received actions.
- Repositories: native in-memory repository interfaces now exist for plants, bunches, shipments, and inventory.
- Domain and application services: native command handlers now orchestrate harvest, ripen, ship, receive, sell, and discard workflows through repository-backed operations.
- Anti-corruption and CQRS support: native adapters translate external telemetry, external inventory records, and external weather observations, and native read models project stock, ripeness, and cultivar statistics.

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
- Native core code lives in explicit domain files under `src/native/core/domain`, with DAL-specific code under `src/native/core/dal`.
- Wrapper ABI remains stable while the domain model keeps replacing the legacy projection path.
- The Banana DDD feature surface described in `docs/banana-ddd.md` is covered inside native core, except for the explicitly out-of-scope consumer context.