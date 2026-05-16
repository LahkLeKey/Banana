<!--
Sync Impact Report
- Version change: 1.0 -> 1.1.0
- Modified principles: XXV amendment procedure strengthened; added XXVII agent jurisdiction; added XXVIII spec supersession and baseline reset
- Added sections: Constitutional Order
- Removed sections: none
- Templates requiring updates: ✅ .specify/templates/spec-template.md, ✅ .specify/templates/plan-template.md
- Follow-up TODOs: none
-->
# Banana Project Constitution

**Version**: 1.1.0 | **Date**: 2026-05-15 | **Status**: Active
**Custodians**: Architecture Council | **Review Cycle**: Quarterly + per baseline reset

---

## Preamble

This constitution establishes the foundational principles, decision-making framework, and architectural contracts for the Banana monorepo. All features, slices, and components must align with these principles to maintain coherence, testability, and reusability across native (C), backend (C#/TypeScript), frontend (React/React Native/Electron), and deployment (Docker/Compose) layers.

**Immutable Principle**: If a principle conflicts with a new requirement, surface the conflict explicitly, propose a principle update, and await approval before breaking the existing contract.

## Constitutional Order

This constitution governs Banana through separated but cooperative powers so scope changes,
implementation changes, and validation changes are not conflated.

- **Legislative Authority**: The constitution, active specs, and templates define scope, terms, and jurisdiction.
- **Executive Authority**: Domain agents and implementers execute only within the scope delegated by the active spec.
- **Judicial Authority**: Reviewers, validators, tests, and CI determine whether executive work complies with the legislative record.

DDD and SOLID rules apply to governance as well as code: each artifact owns one concern, each
agent operates inside a bounded context, and no branch may silently assume powers reserved to
another branch.

---

## Core Principles

### I. Single Domain of Truth

**Statement**: Business logic (game simulation, state transitions, validation) lives exclusively in the native C core (`src/native`). All other layers (React, TypeScript API) are orchestration shells that invoke, display, or persist the core domain.

**Application**:
- ✅ Game entity state (position, health, animation) computed in C
- ✅ ARPG camera logic in C; React only renders canvas
- ✅ Input validation and action dispatch in C
- ✅ React state limited to UI chrome (menu visibility, input focus, overlay status)
- ❌ Game logic decision trees in React components
- ❌ Physics/collision calculations in TypeScript
- ❌ Cheatable state mutations in frontend

**Enforcement**: Code review checklist item; architecture tests validate no game logic in non-native layers.

---

### II. Deterministic Simulation

**Statement**: Game state changes are deterministic given identical input sequences. The same input replay on server and client produces identical output. This is foundational for multiplayer, replay validation, and testing.

**Application**:
- ✅ All random numbers seeded from server tick
- ✅ Floating-point operations consistent across platforms (fixed epsilon, same library versions)
- ✅ No timestamp-based decisions (only tick-based)
- ✅ No ambient randomness in entity ticks
- ✓ Player input ordering is canonical from server
- ❌ Client-side coin flips for damage variation
- ❌ Using `Date.now()` for game logic
- ❌ Platform-specific floating-point differences left unchecked

**Enforcement**: Determinism test suite runs input replay scenarios; any divergence blocks merge.

---

### III. Spec-First Development

**Statement**: Every feature slice begins with a written specification in `.specify/specs/<number>-<short-name>/spec.md`. Specs define purpose, scope, acceptance criteria, and domain contracts before implementation. Plans and tasks flow from specs.

**Application**:
- ✅ Spec exists and links to related specs before PR
- ✅ `plan.md` generated after spec approval
- ✅ Tasks generated from plan; each task traces to plan section
- ✅ Confidence gate passed (≥80%) before major implementation
- ✓ Spec updated with architecture decisions as design emerges
- ❌ Code written before spec exists
- ❌ Specs marked "draft" in production branch
- ❌ Tasks created without traceability to spec sections

**Enforcement**: PR template requires `Related Specs` link; branch protection requires plan.md in scope directory.

---

### IV. Platform-Agnostic Domain Layer

**Statement**: Business domains (input aggregation, interaction routing, viewport sizing, netcode) exported from `@banana/ui` must be platform-agnostic. No React, React Native, Electron, or browser-specific imports in domain code. Presentation adapters exist in target platforms.

**Application**:
- ✅ `InputAggregator` in `InputDomain` takes key strings; no `KeyboardEvent`
- ✅ `InteractionManager` pure function handlers; no React hooks
- ✅ `ViewportDizer` accepts element dependency; works in any DOM-like environment
- ✅ NetcodeDomain, ReplicationDomain use only TypeScript primitives
- ❌ `import React` in any domain file
- ❌ Platform checks like `process.env.REACT_NATIVE_VERSION`
- ❌ HTML canvas assumptions in non-presentation code

**Enforcement**: ESLint rule forbids platform imports in `src/shared/ui/src/domains/`; import paths audited in review.

---

### V. Type Safety at Runtime

**Statement**: TypeScript interfaces are contracts, not hints. Types represent invariants; type correctness = runtime correctness. No `any` types in domain code. Discriminated unions for state machines (not strings).

**Application**:
- ✅ `EngineStatus` as discriminated union: `type EngineStatus = "idle" | "loading" | "running" | "error" | "unavailable"`
- ✅ `GameEngineUIState` fully typed with no optional fields (use null if optional)
- ✅ `ActionHandler` function signature: `(actionId: string, x: number, y: number) => InteractionOutcome`
- ✅ Value objects immutable and readonly fields
- ❌ `{ [key: string]: any }` for game state
- ❌ `let state: any; state.x = 5`
- ❌ Untyped event handlers

**Enforcement**: `tsconfig.json` strict mode enabled; no `as any` without comment explaining why; review checklist includes type coverage.

---

### VI. One-Window Validation

**Statement**: All interactive validation (scene rendering, menu behavior, input responsiveness, multiplayer sync) must be observable and testable in a single VS Code window using integrated browser, terminal, and debugger. No external tools required for local development validation.

**Application**:
- ✅ VS Code task launches Compose stack; browser auto-opens to app
- ✅ Console logs and React DevTools accessible in integrated browser
- ✅ Terminal shows build/test output; can re-run from browser back to code
- ✅ Network tab in DevTools shows WebSocket replication messages
- ✓ Mobile emulator runs in WSL2 with WSLg rendering to same window (Ubuntu WSL2 contract)
- ❌ Requiring external Terminal.app or separate Xcode window
- ❌ "Run on device only" features with no emulator path
- ❌ Hidden network traffic (packet capture only)

**Enforcement**: Local runtime setup documented in `.github/ubuntu-wsl2-runtime-contract.md`; CI validates same codepaths work in compose.

---

### VII. SOLID Principles in Implementation

**Statement**: Code must follow SOLID principles: Single Responsibility, Open/Closed, Liskov Substitution, Interface Segregation, Dependency Inversion. Violations surface as difficult tests, tight coupling, or feature brittleness.

**Application**:
- ✅ `InputAggregator` manages only input aggregation (SRP)
- ✅ `ActionHandler` type allows new actions without modifying `InteractionManager` (OCP)
- ✅ `MobileRadialControl` and `ContextMenu` satisfy interchangeable affordance pattern (LSP)
- ✅ Domains expose minimal, focused methods (ISP)
- ✅ `GameEngineUIService` accepts dependencies; doesn't create them (DIP)
- ❌ `GameEngineController` managing input, rendering, network, persistence (SRP violation)
- ❌ Adding new action type requires modifying menu switch statements (OCP violation)
- ❌ Entity-based component inheritance with mixed concerns (LSP violation)

**Enforcement**: Architecture review checklist; refactoring before feature if SOLID violations exceed threshold.

---

### VIII. Testing Pyramid: Unit > Integration > E2E

**Statement**: Test coverage follows a pyramid: majority unit (fast, isolated), fewer integration (component boundaries), fewest E2E (full stack). Each layer validates a specific contract.

**Application**:
- ✅ Unit: `InputAggregator` test suite (4/4); no network, no React
- ✅ Integration: `InputDomain` ↔ `GameEngineUIService` test (mocked events → state changes)
- ✅ E2E: Two browser tabs connect to local server; sync state over 2min session
- ✓ All unit tests in CI (fast, < 1min per suite)
- ✓ Integration tests in CI (moderate, < 5min)
- ✓ E2E tests in CI (slower, < 30min per profile)
- ❌ Majority of tests are E2E (slow feedback)
- ❌ Unit tests mock too much (testing mocks, not code)
- ❌ No integration layer (domain units tested in isolation but don't compose)

**Enforcement**: Coverage reports per test type; CI fails if unit:integration:E2E ratio inverted.

---

### IX. Server-Authoritative Gameplay (Multiplayer Foundation)

**Statement**: Game state authority lives on the server. Clients receive authoritative updates via replication; local simulation optimistic only. No client-side decisions affect canonical game state without server validation.

**Application**:
- ✅ Server processes input in tick order; publishes deltas to clients
- ✅ Clients apply delta updates from server; reconcile with local prediction
- ✅ Actions (interact, harvest) validated server-side; client-side prediction rolls back if invalid
- ✅ World state (health, inventory, position) replicated from server each tick
- ✅ Single-player mode runs local server instance (same code path)
- ❌ Client accepts damage input directly (client-side cheat surface)
- ❌ "Sync state with server" optional after action (always authoritative)
- ❌ Different validation logic on client and server
- ❌ Trusting client timestamps for action order

**Enforcement**: Network layer contract document; security review for cheating surfaces before release.

---

### X. Delta Replication for Network Efficiency

**Statement**: Network messages contain only changed state fields, not entire world snapshots. Delta encoding reduces bandwidth; enables higher tick rates on limited connections.

**Application**:
- ✅ `DeltaUpdate` contains `{ entityId, changes: { position, health } }` only
- ✅ Client merges deltas into shadow world state
- ✅ Unspecified fields retain previous value (no overwrite with null)
- ✅ Tick number included to detect message reordering
- ❌ Sending full entity state each tick
- ❌ Replicating unchanged fields
- ❌ Separate "full state" sync messages (use tick snapshots for rejoin)

**Enforcement**: Protocol review; network monitor shows ≤50 Kbps per player replication.

---

### XI. Input Buffering and Rollback

**Statement**: Client buffers input commands and predicts outcomes locally. On server tick, client applies server-authoritative state and replays uncommitted input to recover local prediction if divergence detected.

**Application**:
- ✅ `InputBuffer` queues commands with sequence numbers
- ✅ Server acknowledges input; client discards buffered copy
- ✅ On mismatch, client keeps server state + uncommitted input buffer
- ✅ Replay buffered input on new server state (deterministic simulation)
- ✓ Player feels responsive even on 100ms+ latency (prediction hides delay)
- ❌ Waiting for server tick to apply input locally (feels laggy)
- ❌ Clearing input buffer on server ACK (loses uncommitted commands)
- ❌ Non-deterministic rollback causing desync

**Enforcement**: Latency test harness validates ≤60ms felt latency at 100ms network delay.

---

### XII. Immutable Value Objects

**Statement**: Domain value objects (MovementInput, InteractionOutcome, TickSnapshot, DeltaUpdate) are immutable. No mutation after creation. Equality based on content, not identity.

**Application**:
- ✅ `readonly` fields in TypeScript interfaces
- ✅ Factory functions to create with validation
- ✅ Immutable data structures (Immer for collections if needed)
- ✅ Equality tests compare field values, not object reference
- ❌ Mutating value object after creation (e.g., `outcome.timestamp = newTime`)
- ❌ Mutable arrays in value objects passed around
- ❌ Identity-based equality for interchangeable objects

**Enforcement**: Linter enforces `readonly` on exported interfaces; mutation tests catch violations.

---

### XIII. Factory Functions Over Constructors

**Statement**: Domain entities use factory functions (e.g., `createViewportSizer()`) rather than `new` constructors. Factories enable dependency injection, testing, and versioning without exposing implementation details.

**Application**:
- ✅ `export function createViewportSizer(element: HTMLElement): ViewportSizer`
- ✅ Factory validates inputs and throws typed errors
- ✅ Easy to mock in tests by replacing factory
- ✅ Future versioning: `createViewportSizerV2(...)` coexists with V1
- ❌ `export class ViewportSizer { constructor() { ... } }` and `new ViewportSizer()`
- ❌ Exposing implementation details in constructor
- ❌ Cannot inject dependencies

**Enforcement**: All exported entities use factory pattern; constructors private.

---

### XIV. Pub/Sub for State Notifications (Not Mutations)

**Statement**: State changes broadcast via listener callbacks; listeners do not mutate shared state. Listeners are notified of changes; they react by updating their own local state or triggering side effects.

**Application**:
- ✅ `service.onStateChange((state) => { setReactState(state); })`
- ✅ Listener receives read-only state snapshot
- ✅ Multiple listeners can subscribe; all notified on change
- ✅ Unsubscribe function returned; cleanup automatic
- ❌ Listener mutates service state directly
- ❌ Service state modified by multiple sources without synchronization
- ❌ Hidden ordering dependencies between listeners

**Enforcement**: Listeners typed as pure functions; no mutation in handler bodies (lint rule).

---

### XV. Rejection of Platform-Specific Workarounds

**Statement**: Platform-specific code is justified only when alternative is impossible. Each workaround must be documented, contained in a platform adapter, and include a proposal for future consolidation.

**Application**:
- ✅ Mobile emulator setup in `.github/ubuntu-wsl2-runtime-contract.md` with explicit constraints
- ✅ React Native Gesture Handler in platform adapter, not in shared domain
- ✅ Electron IPC bridge in electron/, not in shared components
- ✓ Fallback chains documented (e.g., visualViewport → window → documentElement)
- ❌ Platform checks scattered throughout business logic
- ❌ "Works on web only" without platform adapter
- ❌ Silent failures on unsupported platforms

**Enforcement**: Grep for platform imports in src/shared; review required for fallback chains > 3 levels.

---

### XVI. Confidence Gates and Uncertainty Resolution

**Statement**: Before starting implementation on high-risk changes, assess confidence. If < 80%, halt and resolve uncertainty via research, Q&A, spike, or prototyping before continuing.

**Application**:
- ✅ Multiplayer spec confidence gate before implementation (Phase 2)
- ✅ Question resolution documented in plan.md
- ✅ Risk/assumption/decision (RAD) log maintained per spec
- ✅ Spike/POC validated before committing to full implementation
- ❌ "Let's code and see if it works" on architectural questions
- ❌ Confidence < 50% ignored; work continues anyway
- ❌ Uncertainty resolved during implementation (expensive rework)

**Enforcement**: CI gate checks plan.md for confidence section; PR checklist requires confidence ≥ 80%.

---

### XVII. No Unstructured Debugging

**Statement**: Debugging happens via structured logging, error boundary telemetry, and instrumented tests. Ad-hoc console.log scattered throughout code is forbidden. Production logging enables post-hoc analysis without changing code.

**Application**:
- ✅ Structured logger emits `{ level, timestamp, event, context, error }`
- ✅ Error boundaries capture React component failures with stack trace
- ✅ Network monitor logs all messages (type, tick, delta size, latency)
- ✅ Engine (C side) emits debug logs via `printErr` callback
- ✅ Test failures include full context (inputs, state, expected vs actual)
- ❌ `console.log('here')` scattered in code
- ❌ `debugger` statements in production paths
- ❌ Binary search via trial and error

**Enforcement**: Linter forbids console.log in src/; error cases must emit structured logs.

---

### XVIII. Documentation Follows Implementation

**Statement**: Documentation lives in multiple layers: inline code comments (why), API docs (how), examples (what), and spec files (design). Each layer serves a specific audience and is maintained as code changes.

**Application**:
- ✅ Spec defines architecture decisions and rationale
- ✅ Code comments explain non-obvious implementation choices
- ✅ JSDoc documents public API surface (params, return, throws)
- ✅ Examples in spec show usage patterns
- ✅ README.md per package explains module purpose
- ✅ `.github/instructions/` documents domain-specific conventions
- ✓ PR includes doc updates for public API changes
- ❌ Outdated README (version skew)
- ❌ No comments in complex algorithms
- ❌ Spec describes intent but code does something different

**Enforcement**: PR blocks if public API changes lack JSDoc; spec review happens before code review.

---

### XIX. Semantic Versioning for Public APIs

**Statement**: All exported domains, components, and types follow semantic versioning. Breaking changes require version bump, deprecation period, and migration guide.

**Application**:
- ✅ `@banana/ui@1.0.0` ← fixed major version; patch bumps for features, minor for breaking
- ✅ Deprecation markers: `@deprecated Use X instead` in JSDoc
- ✅ Coexistence: `createInputAggregatorV1` and `createInputAggregatorV2` both exported
- ✅ Migration guide provided 1 release before removal
- ❌ Changing function signature without version bump
- ❌ No deprecation period; direct removal

**Enforcement**: `package.json` version aligned with git tag; breaking changes require version proposal in PR.

---

### XX. Performance Budget and Optimization Discipline

**Statement**: Performance targets (latency, bandwidth, memory) are defined before implementation. Optimizations measured against baselines. Premature optimization forbidden; only optimize validated bottlenecks.

**Application**:
- ✅ Target: ≤ 60ms input latency with prediction
- ✅ Target: ≤ 50 Kbps replication bandwidth per player
- ✅ Target: 30 FPS server tick rate
- ✅ Profiler data (Chrome DevTools, native perf tools) included in performance-related PRs
- ✅ No optimization without supporting measurements
- ❌ Micro-optimizing string concatenation (not the bottleneck)
- ❌ Claiming performance without numbers

**Enforcement**: Performance PR checklist requires before/after metrics; regression tests if threshold exceeded.

---

### XXI. Security by Design, Not Afterthought

**Statement**: Security considerations (input validation, replay prevention, authorization) designed into the system upfront. No "security pass" at end of development.

**Application**:
- ✅ Input validation on both client (UX) and server (authority)
- ✅ Replay attack prevention via sequence numbers and server acknowledgment
- ✅ Session tokens and expiration
- ✅ Rate limiting on API endpoints
- ✅ Cheating surface analysis before feature release
- ❌ "Assuming players won't cheat" logic
- ❌ Trusting client-side timestamps or validation
- ❌ Skipping input sanitization

**Enforcement**: Security checklist per spec; code review includes cheating surface analysis.

---

### XXII. Monorepo Hygiene and Clear Ownership

**Statement**: Each domain/package has clear ownership (native, API, React, mobile, etc.). No ambiguous "should this go here?" decisions. Dependency graph acyclic; layers flow one direction.

**Application**:
- ✅ `src/native/` — Game simulation logic (authoritative)
- ✅ `src/typescript/api/` — Fastify API and netcode domains (orchestration)
- ✅ `src/typescript/react/` — React web app (presentation)
- ✅ `src/typescript/shared/ui/` — Domain entities and platform-agnostic components
- ✅ Dependency direction: native ← API ← React (never reverse)
- ❌ Circular imports (React importing from native imports React)
- ❌ Ambiguous "utilities" folder with mixed concerns
- ❌ Package without clear purpose

**Enforcement**: Dependency audit in CI; graph visualization in architecture docs.

---

### XXIII. Rollback Strategy and Safe Deploys

**Statement**: All features include a rollback plan. Database migrations include down scripts. Feature flags enable gradual rollout and fast rollback without redeployment.

**Application**:
- ✅ Database schema changes include `prisma migrate rollback` steps
- ✅ Server API versioning allows old clients to work during transition
- ✅ Feature flags control new netcode/replication code without code change
- ✅ Canary deployment to 10% of users before 100%
- ❌ "Rollback means re-running the whole deployment"
- ❌ Breaking schema changes with no down migration

**Enforcement**: Rollback plan required in spec; tested in staging before production.

---

### XXIV. Domain-Focused Test Decomposition

**Statement**: Tests organized by domain, not by file. Each domain has a dedicated test suite validating its invariants in isolation. Integration tests validate boundaries between domains.

**Application**:
- ✅ `InputAggregator.test.tsx` — 4 tests, all input aggregation logic
- ✅ `InteractionManager.test.tsx` — 4 tests, all action routing logic
- ✅ `ViewportSizer.test.tsx` — 3 tests, all sizing logic
- ✅ `GameEngineUIService.test.tsx` — 3 tests, all orchestration logic
- ✅ `GameEnginePage.integration.test.tsx` — 5 tests, all domains working together
- ❌ Random test files mirroring directory structure
- ❌ Test per React component (tests presentation, not logic)
- ❌ No clear relationship between test and domain

**Enforcement**: Test file naming convention: `<Domain>.test.tsx`; review checks test organization.

---

### XXV. Constitutional Amendment and Continuing Authority

**Statement**: This constitution is living but not informal. Principles can be updated, added, superseded, or repealed only through an explicit amendment record approved by the Architecture Council and reflected in the governing templates and active baseline specs.

**Application**:
- ✅ Quarterly review of all principles
- ✅ Proposal issues or specs raised for new principles and jurisdiction changes
- ✅ Accepted principles propagate into templates and active baseline specs in the same change
- ✅ Deprecation period for removed principles (1 release) unless a security emergency demands immediate replacement
- ✅ Amendments record who proposed, who ratified, and which active specs were affected
- ❌ Constitution treated as optional commentary
- ❌ New principles added ad-hoc without review or propagation

**Enforcement**: Constitutional changes in separate PR; tagged as `constitution-update`.

---

### XXVI. Native Usermode Anti-Cheat and Multi-Layer Heartbeats

**Statement**: Anti-cheat enforcement in the server-authoritative path MUST execute in native C for hot-path scoring and MUST remain usermode-only. Detection of suspicious ring-1/ring-2 class activity is treated as telemetry-driven risk scoring, not kernel tampering.

**Application**:
- ✅ Native scoring consumes usermode heartbeat telemetry (sequence, tick monotonicity, integrity anomalies)
- ✅ Native scoring consumes suspicious driver telemetry (ring-1/ring-2 class indicators, unsigned drivers, hidden modules)
- ✅ Multi-layer heartbeats tracked independently (transport, session, simulation/integrity)
- ✅ Risk model uses bounded O(1) operations per heartbeat and fixed-size session state
- ✅ Security actions are policy-based (throttle, challenge, quarantine), not immediate hard-kill by default
- ❌ Kernel hooks, unsigned kernel drivers, or patch-guard hostile techniques
- ❌ Treating usermode telemetry as hard proof without confidence scoring
- ❌ Unbounded per-session memory growth or expensive deep scans in tick loop

**Enforcement**: Native anti-cheat regression tests must pass; spec slices must document telemetry sources, thresholds, and false-positive controls.

---

### XXVII. Separated Powers, Agent Jurisdiction, and Delegated Authority

**Statement**: Every active spec MUST declare its jurisdiction, bounded context, and agent of record. Agents may execute code, tests, and document updates within that jurisdiction, but only legislative artifacts (constitution, active spec, templates) may enlarge scope or redefine terms.

**Application**:
- ✅ Active spec declares the owning bounded context and agent of record before implementation begins
- ✅ Multiplayer authority belongs to the spec that governs server authority, prediction, replication, and anti-cheat as one coherent domain
- ✅ Domain agents update code and tasks within delegated authority and append heartbeat evidence for material decisions
- ✅ Cross-domain changes require an explicit amendment or supersession note in the active spec
- ❌ Agent silently broadens scope beyond the active spec
- ❌ Multiple active specs claim the same authoritative multiplayer jurisdiction
- ❌ Templates omit jurisdiction and agent-of-record fields

**Enforcement**: Spec review rejects work without explicit jurisdiction; templates and active specs must expose the responsible agent and affected bounded context.

---

### XXVIII. Spec Supersession, Consumption, and Baseline Reset

**Statement**: When a later spec fully incorporates the governing scope of earlier slices, the earlier slices MUST be consumed and archived so only one active baseline remains authoritative for that program. Active spec roots are for current law, not historical accumulation.

**Application**:
- ✅ Spec 050 may absorb earlier player-controller and ARPG-view foundations when multiplayer authority becomes the governing baseline
- ✅ Superseded specs move to `.specify/legacy-baseline/specs-archive/` in the same change that updates the active baseline
- ✅ `.specify/specs/README.md` and `.specify/feature.json` align to the sole active baseline after consolidation
- ✅ Historical specs remain preserved in archive for audit and lineage, not for bootstrap context
- ❌ Archived guidance left in the active spec root
- ❌ Multiple near-duplicate baseline specs force agents to read conflicting histories before acting
- ❌ Feature pointer references a spec outside the active baseline set

**Enforcement**: Baseline reset PRs must archive superseded specs, update lineage in the surviving spec, and leave the active root at the minimum set needed for current execution.

---

## Articles of Governance and Relief

### Petitions for Review

Escalate architectural decisions to the architecture team if:
1. **Principle Conflict** — New requirement contradicts existing principle
2. **Domain Boundary Unclear** — Feature doesn't fit neatly in existing package
3. **Dependency Reversal Risk** — Feature would add backward dependency
4. **Performance Impact** — Feature changes performance tier (e.g., O(n) → O(n²))
5. **Security Surface** — Feature introduces new security consideration
6. **Confidence < 80%** — Uncertainty prevents clear design

**Procedure**:
1. Open architecture discussion issue
2. Propose 2-3 alternatives with pros/cons
3. Architecture team reviews; decision recorded in issue
4. Implementation proceeds with approved approach

---

## Judicial Review and Enforcement

### Code Review Checklist

- [ ] Spec linked and in scope
- [ ] SOLID principles applied
- [ ] Type safety maintained (no `any`)
- [ ] Tests follow pyramid (unit > integration > E2E)
- [ ] No platform-specific code outside adapters
- [ ] Value objects immutable
- [ ] Factories used instead of constructors
- [ ] Documentation updated (JSDoc, spec, etc.)
- [ ] No performance regression (measured)
- [ ] Security checklist passed (input validation, cheating surface)
- [ ] Native anti-cheat telemetry path validated (usermode + suspicious driver signals)
- [ ] Confidence gate passed (≥ 80%)

### Sessions and Cadence

- **Quarterly**: Constitution review, principle updates
- **Per-Spec**: Architecture decisions, domain boundaries
- **Per-PR**: Code organization, SOLID compliance, cross-cutting concerns

---

## Appendix: Glossary

- **Domain**: Cohesive unit of business logic with clear boundaries and invariants (e.g., InputDomain)
- **Entity**: Stateful domain object with identity (e.g., InputAggregator)
- **Value Object**: Immutable domain object representing a concept (e.g., MovementInput)
- **Tick**: Discrete time step in server simulation (typically 33.3ms at 30 FPS)
- **Delta**: Only the changed fields in entity state
- **Replication**: Server broadcasts state changes to clients
- **Prediction**: Client-side optimistic simulation pending server confirmation
- **Authority**: The source of truth (server is authoritative; client is eventual-consistency)
- **Determinism**: Same input sequence always produces same output
- **Platform Adapter**: Platform-specific wrapper around domain (e.g., React Native gesture handler)
- **Usermode Telemetry**: Non-kernel signals collected from process/runtime space for risk scoring
- **Ring-1/Ring-2 Suspicious Signal**: Hypervisor/driver-class anomaly indicators consumed as weighted telemetry, not as direct kernel control

---

## Approval

**Constitution Approved By**: Architecture Team
**Date**: 2026-05-15
**Next Review**: 2026-08-15
**Version Control**: `.specify/constitution.md` (tracked in git)

---

**This constitution is the source of truth for Banana project architecture. All features, PRs, and decisions reference these principles.**
