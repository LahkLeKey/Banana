# Feature Specification: Client Static Mesh Generators and Banana Line Travel

**Feature Branch**: 010-client-static-mesh-banana-line

**Created**: 2026-05-25

**Status**: Draft

**Input**: User description: Build client-shipped static mesh generators for 8 counties plus islands, keep server storage focused on procedural inputs and player deltas, and add Banana Line travel between generated regions.

**Canonical Baseline**: docs/banana-archipelago-worldbuilding.md is the authority for region/county naming and biome adjacency constraints.

## User Scenarios & Testing (mandatory)

### User Story 1 - Generate County Mesh Baselines on Client (Priority: P1)

As a player exploring the overworld, I can stream deterministic county terrain meshes from client-side generators so server storage only tracks authoritative procedural inputs and edits.

**Why this priority**: This is the storage gate for supporting the 348,200 km2 map under a 10 GB total database ceiling.

**Independent Test**: Can be tested by generating a county hub chunk and nearby chunks twice from equivalent profile inputs and verifying parity signatures.

**Acceptance Scenarios**:

1. Given a county static-mesh profile and local chunk coordinate, when generation runs twice on equivalent inputs, then output heightfields and signatures match.
2. Given different county profiles, when generation runs at profile hub coordinates, then generated signatures differ.

---

### User Story 2 - Travel Between Regions on Banana Line (Priority: P1)

As a player moving between counties and islands, I can follow deterministic Banana Line routes between region hubs so traversal paths are stable for gameplay events and streaming.

**Why this priority**: Stable travel lanes provide a predictable backbone for prefetching, world event scheduling, and multiplayer rendezvous.

**Independent Test**: Can be tested by routing between county and islands profiles and verifying route start/end alignment and deterministic step count.

**Acceptance Scenarios**:

1. Given a source and destination region profile, when Banana Line routing is requested, then the route starts at source hub and ends at destination hub.
2. Given equivalent source and destination profiles, when routing is requested repeatedly, then step coordinates are deterministic matches.

---

### User Story 3 - Server Storage Safety Under Gameplay Ledger Growth (Priority: P2)

As an operator, I can cap server persisted terrain payloads to procedural inputs and player deltas so progression ledgers and gameplay state remain within the 10 GB system budget.

**Why this priority**: The project budget constraint applies to total DB usage, not only terrain data.

**Independent Test**: Can be tested by persistence contract checks that reject baseline mesh blobs and accept only seeds, profile IDs, route IDs, and delta/version records.

**Acceptance Scenarios**:

1. Given a generated client mesh baseline, when persistence is requested, then server stores only profile, seed, version, and delta metadata.
2. Given high player edit volume plus progression ledger growth, when periodic storage audits run, then total DB footprint remains under configured thresholds.

### Edge Cases

- Banana Line route exceeds allowed route-step capacity for a low-memory client.
- Islands profile requires crossing long water gaps where streaming windows differ from inland counties.
- Player reconnects mid-route while source chunk cache is evicted.
- Client profile version mismatches server expected generator version.

## Requirements (mandatory)

### Functional Requirements

- FR-001: System MUST define static mesh profiles for exactly the 8 canonical regions in docs/banana-archipelago-worldbuilding.md.
- FR-002: System MUST generate county and islands baseline terrain deterministically from profile, world ID, lane ID, and chunk coordinates.
- FR-003: System MUST provide a deterministic Banana Line route function between profile hub chunks.
- FR-004: System MUST expose generator version metadata to detect client and server profile drift.
- FR-005: System MUST keep server persistence limited to procedural contracts, authoritative deltas, and progression ledger records.
- FR-006: System MUST reject attempts to persist full baseline mesh payloads in the authoritative server database.
- FR-007: System MUST provide playtest validation steps to execute generator and route checks per profile increment.
- FR-008: System MUST record storage-budget evidence for terrain deltas and progression ledgers under a 10 GB total DB budget.

### Key Entities

- Static Mesh Profile: Region-level generation contract containing profile ID, Banana Line lane ID, hub chunk coordinates, coverage radius, and partition epoch.
- Banana Line Route: Deterministic ordered chunk-step path between two profile hubs.
- Client Mesh Baseline: Regenerable terrain output produced client-side from static profile contracts.
- Authoritative Delta Ledger: Server-side sequence of player edits and progression state transitions against area versions.

## Success Criteria (mandatory)

### Measurable Outcomes

- SC-001: 100% deterministic parity across repeated generation of sampled profile/chunk pairs in native playtest suite.
- SC-002: 100% deterministic parity for Banana Line route step sequences across repeated source/destination runs.
- SC-003: Server-side DB storage for terrain-specific records remains focused on procedural inputs and deltas with zero persisted baseline mesh blobs.
- SC-004: Combined terrain-delta and progression-ledger storage stays within the 10 GB cap in budget simulations.

## Assumptions

- Full baseline terrain meshes are distributed as client assets or generated client-side, not stored as authoritative DB blobs.
- Banana Line travel is represented as chunk-space route paths for initial implementation.
- Existing deterministic terrain generation contracts remain the baseline generator for static profile scaffolding.
- Follow-up API persistence enforcement work can be staged after native generator and playtest scaffolding.

## Constitution Alignment (mandatory)

- Disclosure Integrity: Player-facing claims focus on persistent shared-world edits and deterministic traversal consistency, not server-stored full terrain mesh payloads.
- Cross-Domain Contracts: Native terrain generator contracts, client runtime streaming routes, and API/server persistence boundaries are all affected.
- Quality Gates: Native deterministic generation tests, route determinism tests, and storage-policy validation checks are required.
- Delivery Evidence: Native playtest outputs, deterministic parity logs, and storage-budget snapshots are required artifacts for merge readiness.
