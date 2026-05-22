# Feature Specification: Class Builds, Gear Upgrades, and Skill Combinations

**Feature Branch**: `001-steamworks-release-readiness`

**Created**: 2026-05-21

**Status**: In Progress (Gameplay Slice Implemented)

**Input**: User description: "Shape your character with class-driven builds, gear upgrades, and skill combinations designed for both solo play and party-based runs."

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Build a Class Identity (Priority: P1)

As a player, I want to pick a class and make meaningful build choices so my character
feels distinct in both combat approach and progression.

**Why this priority**: Character identity is the core gameplay loop and is required before
gear and combo systems can deliver value.

**Independent Test**: A player can create a character, choose a class path, and verify
class-specific attributes and available skills differ from other classes.

**Acceptance Scenarios**:

1. **Given** a new character, **When** the player selects a class, **Then** the system assigns class-specific starter stats, starting skill set, and build path options.
2. **Given** two different classes, **When** both enter the same encounter, **Then** class toolkit differences are observable in combat role and effectiveness profile.

---

### User Story 2 - Progress Through Gear Upgrades (Priority: P2)

As a player, I want gear to provide meaningful upgrades and tradeoffs so progression
feels rewarding instead of a flat stat increase.

**Why this priority**: Gear progression is the primary medium-term retention loop and needs
to reinforce class identity.

**Independent Test**: A player can earn, equip, and compare gear upgrades while seeing
clear, measurable changes to combat performance and build tradeoffs.

**Acceptance Scenarios**:

1. **Given** a player acquires higher-tier gear, **When** the gear is equipped, **Then** derived stats and relevant skill scaling update immediately.
2. **Given** two gear loadouts for the same class, **When** the player swaps loadouts, **Then** combat outcomes reflect expected tradeoffs such as survivability versus burst damage.

---

### User Story 3 - Chain Skills for Solo and Party Runs (Priority: P3)

As a player, I want skill combinations that support both solo efficiency and party
synergy so I can adapt my build to different play contexts.

**Why this priority**: Skill synergy is the bridge between personal build mastery and
multiplayer cooperation.

**Independent Test**: Players can execute combo chains in solo and party scenarios and
observe distinct outcomes for damage windows, control, and team utility.

**Acceptance Scenarios**:

1. **Given** a solo run, **When** the player executes a supported skill sequence, **Then** the system applies combo bonuses according to declared sequence rules.
2. **Given** a party run, **When** players chain compatible skills across classes, **Then** party-wide synergy effects trigger within defined timing windows.

---

## Edge Cases

- A player attempts to equip gear that conflicts with class restrictions.
- A combo sequence input is partially correct but misses the timing window.
- Party members apply overlapping synergy effects that could stack incorrectly.
- A solo-optimized build enters party content and risks invalidating encounter balance.
- Gear level jumps create abrupt power spikes that bypass intended progression pacing.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The system MUST provide at least three distinct class archetypes with unique role identity and class-specific progression paths.
- **FR-002**: The system MUST allow players to allocate class build choices that materially change skill behavior or combat role.
- **FR-003**: The system MUST define gear tiers and upgrade paths with predictable stat progression and explicit tradeoffs.
- **FR-004**: The system MUST recalculate derived stats and affected skill effects immediately after gear equip/upgrade changes.
- **FR-005**: The system MUST support skill-combo definitions that include sequence order, timing window, and resulting effects.
- **FR-006**: The system MUST support both solo and party validation paths for combos, including party synergy triggers across classes.
- **FR-007**: The system MUST surface build and gear impact feedback to players for decision clarity (for example damage, survivability, utility deltas).
- **FR-008**: The system MUST enforce balance constraints that prevent invalid gear/class combinations and uncontrolled synergy stacking.

### Key Entities *(include if feature involves data)*

- **Class Archetype**: A player class with base attributes, role identity, and class-specific skill tree options.
- **Build Configuration**: A chosen set of class allocations, equipped gear, and selected skill loadout.
- **Gear Item**: Equipment with slot, tier, modifiers, upgrade level, and compatibility constraints.
- **Skill Combo Definition**: A rule set describing sequence order, timing windows, and combo outcomes.
- **Synergy Effect**: A cross-player or cross-skill interaction outcome active under defined conditions.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: At least 90% of test players can identify meaningful gameplay differences between class archetypes within 15 minutes of play.
- **SC-002**: Gear upgrades produce statistically significant but bounded performance gains across defined tier transitions in test scenarios.
- **SC-003**: At least 85% of validated combo sequences trigger correctly in both solo and party test runs.
- **SC-004**: No critical balance defect allows prohibited class/gear combinations or unintended infinite synergy loops in integration tests.
- **SC-005**: At least 80% of surveyed test players report that build decisions feel clear and impactful.

## Assumptions

- Existing combat, inventory, and encounter systems are available as integration surfaces for this feature.
- Initial implementation scope targets a stable subset of classes and gear slots before full content expansion.
- Skill combo timing windows can be represented deterministically across supported runtimes.
- Party-run validation is available in at least one repeatable multiplayer test environment.

## Constitution Alignment *(mandatory)*

- **Disclosure Integrity**: Player-facing class, gear, and combo behavior claims must match in-game outcomes and public-facing descriptions.
- **Cross-Domain Contracts**: Any class/gear/combo rule changes must keep native simulation, API payloads, and client rendering behavior aligned.
- **Quality Gates**: Unit, integration, and multiplayer validation must verify deterministic combo outcomes, compatibility constraints, and balance safeguards.
- **Delivery Evidence**: Test evidence must include solo and party run logs, build configuration snapshots, and balance validation outputs.

## Implementation Evidence (2026-05-21)

- Native gameplay build/combo runtime implemented and validated through focused C tests.
- API routes for class, allocations, gear equip, build stats, and combo evaluation implemented with request validation coverage.
- React UI integration implemented in the overworld Build Lab panel and validated through component and API client tests.
- Visual runtime verified at `/session-room` and `/game-engine`, including successful interactive Build Lab actions:
	- class change,
	- allocation preset apply,
	- gear equip,
	- combo evaluation.

### Current Validation Snapshot

- Native focused tests: `ctest --test-dir out/v3-native -C Debug -R "player_builds|runtime_player_builds_integration" --output-on-failure`.
- API route tests: `bun test src/routes/game-session.test.ts`.
- React gameplay tests: `bun test src/lib/api.test.ts src/components/game/OverworldHud.test.tsx`.
- Runtime smoke: API health and visual route verification on local runtime channel.
