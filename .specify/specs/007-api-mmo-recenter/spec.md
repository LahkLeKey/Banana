# Feature Specification: API MMO Recenter

**Feature Branch**: `[007-api-mmo-recenter]`

**Created**: 2026-05-24

**Status**: Draft

**Input**: User description: "Our API has drift. Re-center it to orchestrate MMO gameplay elements used by the DX12 client while also providing website-focused player APIs. Include deployment intent for Fly.io, with Neon serverless Postgres as the database system of record for all persistent data. Use domain-driven design with explicit subdomains."

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Realtime Session Orchestration for Connected Players (Priority: P1)

As an active player connected through the DX12 client, I need gameplay session state to be orchestrated consistently by the API so that movement, matchmaking/session membership, and in-session world interactions remain synchronized and authoritative.

**Why this priority**: This is the core MMO gameplay value path. Without reliable session orchestration, multiplayer runtime behavior is not trustworthy.

**Independent Test**: Can be fully tested by running a multi-client gameplay session where each client performs session join, state-changing actions, and state reads, and all clients observe consistent session outcomes with no divergent persistent records.

**Acceptance Scenarios**:

1. **Given** two players are authenticated and eligible for a gameplay session, **When** both join the same session and perform session actions, **Then** both clients receive consistent session state progression and authoritative conflict resolution from the API.
2. **Given** a player disconnects during an active session, **When** that player reconnects within the allowed continuity window, **Then** the API restores the player to the correct recoverable session state according to session rules.

---

### User Story 2 - Unified Player Account and Progression Across Client and Web (Priority: P2)

As a player using both the DX12 client and the website, I need one identity and progression record so that account status, inventory, and progression are unified regardless of channel.

**Why this priority**: Shared identity and progression prevents channel drift and preserves trust that player outcomes are persistent and portable.

**Independent Test**: Can be fully tested by changing progression and inventory in one channel, then validating that the other channel reflects the same authoritative state without manual reconciliation.

**Acceptance Scenarios**:

1. **Given** a player earns progression rewards through gameplay, **When** the player views account progression on the website, **Then** the website reflects the same progression state committed by gameplay orchestration.
2. **Given** an account update is performed from the website, **When** the player re-enters gameplay through the DX12 client, **Then** session start uses the updated authoritative account and inventory state.

---

### User Story 3 - Website Player Insights and Self-Service Visibility (Priority: P3)

As a website player, I need access to player-facing insights such as session history summaries, progression milestones, and inventory trends so that I can understand my gameplay outcomes and decide next actions.

**Why this priority**: Insights increase retention and reduce support load, but they depend on reliable orchestration and persistence from higher-priority stories.

**Independent Test**: Can be fully tested by generating gameplay and account activity, then confirming the website exposes accurate, player-readable insights derived from the same persisted source of record.

**Acceptance Scenarios**:

1. **Given** a player has completed multiple gameplay sessions, **When** the player opens web insights, **Then** the API returns session and progression summaries that match persisted records.
2. **Given** no prior gameplay data exists for a new player, **When** the player opens web insights, **Then** the API returns an empty but valid response with clear no-data semantics.

---

### Edge Cases

- What happens when the same player account performs overlapping mutating actions from the DX12 client and website at nearly the same time?
- How does the system handle partial service disruption while maintaining authoritative persistence and preventing duplicate progression or inventory grants?
- What happens when a gameplay session ends unexpectedly while pending persistence operations exist?
- How does the system enforce read/write separation so website insight traffic cannot degrade gameplay session orchestration quality under load spikes?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST define and enforce explicit domain-driven subdomains with bounded responsibilities at minimum for gameplay session orchestration, player identity/account, progression/inventory, and web player insights.
- **FR-002**: System MUST provide API capabilities that orchestrate authoritative MMO gameplay session lifecycle events used by the DX12 client, including session admission, in-session state updates, and session termination outcomes.
- **FR-003**: System MUST ensure player identity and account records are shared across DX12 client and website channels with a single authoritative lifecycle for account status and profile ownership.
- **FR-004**: System MUST ensure progression and inventory updates are persisted exactly once per accepted gameplay or web action outcome, with deterministic conflict resolution when concurrent updates occur.
- **FR-005**: System MUST provide website-focused player API capabilities for account visibility, progression visibility, inventory visibility, and player-facing insights derived from authoritative persisted data.
- **FR-006**: System MUST keep gameplay orchestration APIs and website player APIs separately governed at the domain boundary level while preserving a consistent underlying player truth model.
- **FR-007**: System MUST use Neon serverless Postgres as the system of record for all persistent domain data, including account, progression, inventory, and session history required for continuity and insights.
- **FR-008**: System MUST include deployment intent and readiness criteria targeting Fly.io environments for API operation, including expected runtime behavior across staging and production channels.
- **FR-009**: System MUST provide auditable domain event or change-history visibility sufficient to diagnose session, progression, and account state transitions across both client and website access paths.
- **FR-010**: System MUST expose versioned, backward-compatible player-facing API contracts for the DX12 client and website to reduce drift risk during iterative releases.
- **FR-011**: System MUST define domain-level error semantics for transient failures, validation failures, and authorization failures so both client and website channels receive predictable outcomes.
- **FR-012**: System MUST enforce authorization boundaries so players can access only their own account, progression, inventory, and personal insights while allowing gameplay orchestration actions required for active sessions.

### Key Entities *(include if feature involves data)*

- **GameplaySession**: Represents a multiplayer runtime session and its lifecycle; includes session identity, participant roster, authoritative state snapshot metadata, continuity window status, and completion outcome references.
- **PlayerAccount**: Represents player identity and account ownership; includes account identifier, authentication linkage, status, profile attributes, and policy state.
- **ProgressionLedger**: Represents authoritative progression events and milestone outcomes; includes player reference, progression dimensions, event timestamps, and conflict-resolution markers.
- **InventoryLedger**: Represents item ownership and state transitions; includes player reference, item identity, quantity/state, source event linkage, and settlement status.
- **PlayerInsightView**: Represents player-facing aggregate insight data; includes session summary metrics, progression trends, inventory trend indicators, and freshness metadata.
- **DomainChangeRecord**: Represents auditable domain transition history for accountability and troubleshooting; includes bounded-context source, action type, actor scope, before/after summary, and recorded time.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: In validation sessions with concurrent active players, at least 99.5% of accepted gameplay state-changing actions are reflected consistently to all relevant participants within 2 seconds of acceptance.
- **SC-002**: For a representative cross-channel test suite, 100% of sampled player account, progression, and inventory records match between DX12 client-visible outcomes and website-visible outcomes after synchronization completes.
- **SC-003**: At least 95% of players in usability testing can locate their latest progression status and session summary on the website in under 30 seconds.
- **SC-004**: During staged failure injection, duplicate progression or inventory grants occur in 0% of accepted test transactions.
- **SC-005**: Production-readiness checks demonstrate deployment fitness for Fly.io with zero unresolved critical blockers related to startup, health visibility, persistence connectivity, or rollback readiness.
- **SC-006**: API drift reduction objective is met when all in-scope DX12 gameplay orchestration and website player capabilities map to declared bounded subdomains with no undocumented cross-subdomain ownership overlap at release sign-off.

## Assumptions

- Existing channel clients (DX12 and website) remain in scope for this feature and continue to consume API contracts rather than direct persistence access.
- Fly.io is the target deployment platform for API runtime environments and is available for staging and production deployment channels.
- Neon serverless Postgres is available and approved as the sole persistent source of record for this feature scope.
- Real-time low-latency transport specifics are outside this specification and can vary, provided user-facing consistency and responsiveness criteria are met.
- Anti-cheat, payment, and storefront commerce processing remain out of scope unless required by downstream feature work.
- Historical records needed for player insights are retained long enough to satisfy product and support use cases.

## Constitution Alignment *(mandatory)*

- **Disclosure Integrity**: This feature changes internal API orchestration and player-facing API reliability but does not directly alter current public storefront claims. Any future claims about multiplayer synchronization quality or player insights availability must reference measured outcomes in this specification before publication.
- **Cross-Domain Contracts**: Affected contracts span API bounded contexts (gameplay session orchestration, player identity/account, progression/inventory, web player insights), DX12 client consumption contracts, website player consumption contracts, deployment/runtime contracts for Fly.io, and persistence contracts for Neon serverless Postgres.
- **Quality Gates**: Require domain-level unit validation per bounded context, cross-context integration validation for account/progression/inventory consistency, multi-client gameplay orchestration runtime validation, website player insights validation, failure-path validation for concurrent updates, and deployment readiness checks for Fly.io runtime health and rollback behavior.
- **Delivery Evidence**: Evidence must include bounded-context ownership matrix, API contract inventory and compatibility report, cross-channel consistency validation outputs, failure-injection duplicate-prevention results, and deployment readiness artifacts proving Fly.io target fitness with Neon-backed persistence continuity.

## Storefront & Release Claims

- **Storefront Copy Contract**: No storefront copy change is introduced by this feature scope. If release messaging references improved multiplayer orchestration or player insights, the claim must be added through the storefront copy contract update workflow before publication.
- **AI Disclosure**: No AI disclosure changes are required by this feature as scoped.
- **Controller Support**: No controller support claim changes are required by this feature as scoped.
- **System Requirements**: No player-facing minimum system requirement changes are introduced by this feature as scoped.
- **Store Assets**: No new storefront asset requirements are introduced by this feature as scoped.
