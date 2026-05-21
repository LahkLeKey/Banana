# Spec 050 Drilldown: Native Anti-Cheat Pipeline

**Principle:** Client telemetry is treated as probabilistic evidence, not proof. Authoritative enforcement decisions should prefer server-owned simulation validation, multi-domain corroboration, and statistical persistence over isolated client-reported anomalies.

## Goal

Define a compute-efficient native anti-cheat path for the server-authoritative multiplayer pipeline using usermode protections and suspicious ring-1/ring-2 class telemetry.


## Threat Model

The anti-cheat pipeline assumes attackers may possess:

- Usermode memory editing capability
- DLL injection capability
- Replay and packet manipulation capability
- Usermode API hooking capability
- Timing manipulation capability
- Virtualization-assisted cheats
- Signed but suspicious driver-assisted cheats
- Emulator or telemetry fabrication capability

The pipeline does not assume:
- Trusted client clocks
- Trusted local process integrity
- Trusted local telemetry generation
- Trusted packet ordering

The server remains the primary authority for:
- Simulation state
- Trust progression
- Enforcement actions
- Behavioral validation

This becomes extremely valuable later during implementation review.

## Trust Domains

### Untrusted Domain
- Client-generated telemetry
- Timing claims
- Driver enumeration claims
- Integrity self-reports

### Semi-Trusted Domain
- Transport continuity
- Challenge-response validation
- Session continuity state

### Trusted Domain
- Server simulation state
- Authoritative movement state
- Server-side replay analysis
- Policy orchestration
- Risk persistence

This prevents future accidental over-trust.

## Risk Domains

Risk signals are tracked independently before aggregation:

- usermode integrity risk
- kernel/driver telemetry risk
- gameplay behavior risk
- transport/network risk
- timing consistency risk
- challenge integrity risk

No single domain should independently trigger irreversible enforcement unless explicitly configured.

This significantly reduces false positives.

## Behavioral Analytics Layer

The server-authoritative layer should maintain independent behavioral heuristics including:

- movement acceleration plausibility
- aim trajectory analysis
- recoil compensation analysis
- impossible inventory interaction timing
- impossible reaction-time windows
- geometry/pathing violations
- statistically abnormal action frequency

Behavioral analysis operates independently from client integrity telemetry and remains authoritative.

This is strategically the most important part of modern anticheat systems.

## Adaptive Policy Escalation

Policy responses should escalate progressively based on:

- trust tier
- risk persistence
- corroborating domains
- challenge failures
- historical session behavior

Example escalation path:

1. Observe
2. Warn
3. Increase telemetry frequency
4. Issue active challenge
5. Restrict matchmaking trust
6. Throttle simulation authority
7. Quarantine
8. Remove session

This helps avoid brittle binary enforcement.

## Telemetry Authenticity

Heartbeat payloads should support:

- per-session cryptographic keys
- rolling nonces
- replay-resistant counters
- bounded timestamp validity windows
- keyed MAC validation

Replayable or unsigned telemetry must never be considered authoritative integrity evidence.

## Observability and Auditability

The anti-cheat pipeline should emit structured audit events for:

- threshold crossings
- replay rejections
- challenge failures
- trust tier transitions
- integrity hash transitions
- quarantine actions

Audit records should support deterministic replay reconstruction for post-incident analysis.

## Statistical Baseline Learning

Risk thresholds should support adaptive tuning using rolling population baselines:

- average heartbeat jitter
- normal driver telemetry frequency
- typical packet loss ranges
- simulation drift distributions
- hardware variance distributions

Static thresholds should remain configurable overrides rather than sole enforcement criteria.

## Resource Exhaustion Protections

The pipeline must protect against:

- session slot exhaustion
- replay cache flooding
- telemetry spam bursts
- challenge amplification abuse
- intentionally colliding session identifiers

Mitigations may include:

- bounded replay windows
- rate limiting
- probabilistic cleanup
- randomized hashing seeds
- per-IP/session quotas

## Privacy and Data Retention

Telemetry collection should remain limited to anti-cheat operational needs.

The pipeline should avoid:
- unrelated personal data collection
- filesystem scraping
- arbitrary process harvesting
- persistent device fingerprinting beyond configured policy

Retention windows for audit telemetry should remain configurable.

## Active Simulation Challenges

The server may issue randomized challenges including:

- simulation checksum requests
- deterministic state verification
- timing-bound challenge responses
- randomized integrity probes

Challenge results are treated as probabilistic evidence and combined with existing trust-domain scoring.

## Anti-Emulation Considerations

The pipeline should assume advanced attackers may emulate:

- clean heartbeat timing
- expected telemetry distributions
- integrity responses
- replay-safe packet structures

Detection should therefore prioritize:
- behavioral plausibility
- entropy analysis
- cross-domain correlation
- long-horizon statistical anomalies

## Multi-Horizon Risk Tracking

Risk accumulation should support multiple windows:

- short-horizon burst detection
- medium-horizon behavioral trends
- long-horizon trust degradation

Low-and-slow anomalies should remain detectable even when individual bursts decay below enforcement thresholds.

## Failure Semantics

Telemetry parsing or validation failures must:

- fail closed for malformed payloads
- fail open for temporary transport interruption
- avoid irreversible enforcement from isolated parser faults
- preserve auditability of rejected payloads

## Protocol Versioning

Heartbeat payloads should support:

- protocol version identifiers
- feature capability negotiation
- backward-compatible parsing
- optional telemetry extensions

Unknown telemetry fields should be safely ignorable unless marked mandatory.

## Distributed Deployment Considerations

If deployed across multiple simulation nodes:

- session trust state should remain transferable
- replay caches should support bounded synchronization
- challenge state should remain node-consistent
- integrity snapshots should support deterministic replication


- Native scoring core in C with fixed-size session store
- Usermode heartbeat telemetry lane
- Suspicious driver telemetry lane
- Multi-layer heartbeat model
- Policy-facing outputs for server decisions

## Native Contracts

## Session Identity

- Session key: stable server session id
- Session state: rolling score, heartbeat sequence state, integrity hash
- Complexity target: O(1) scoring work per telemetry event

## Usermode Heartbeat Lane

Inputs per heartbeat:
- heartbeat sequence
- simulation tick
- debugger present (0/1)
- code integrity violations (count)
- memory map anomalies (count)
- timing anomalies (count)

Expected checks:
- sequence regression or abnormal jump
- simulation tick regression or extreme skew
- weighted anomaly scoring
- score decay on clean heartbeats

## Suspicious Driver Lane (Ring-1/Ring-2 Class Telemetry)

Inputs per heartbeat:
- suspicious ring-1/ring-2 class driver count
- unsigned driver count
- hidden module count

Expected checks:
- weighted scoring for suspicious driver indicators
- heartbeat monotonicity for driver lane
- integrity hash update per lane event

## Multi-Layer Heartbeat Model

- transport layer heartbeat: connectivity and latency continuity
- session layer heartbeat: player/session lifecycle continuity
- simulation/integrity heartbeat: anti-cheat sequence and hash continuity

## Policy Outputs

- risk score (integer)
- flagged state (threshold crossing)
- rolling integrity hash

Policy actions should remain server-configurable:
- warn
- challenge
- throttle
- quarantine

## Safety and Constraints

- Usermode-only implementation in this slice
- No kernel hooks, no unsigned kernel components
- Driver data treated as telemetry signals, not absolute proof
- Thresholds tuned for false-positive minimization

## Validation Matrix

- Clean lane baseline remains unflagged
- Sequence regression increases score
- Tick regression increases score
- Driver telemetry bursts increase score rapidly
- Integrity hash changes with each accepted telemetry event
- Invalid telemetry payloads return BANANA_INVALID_ARGUMENT

## Hardening Roadmap

The current baseline stays lightweight and server-authoritative, but the following follow-on defenses are the highest-value additions for raising attacker cost without expanding the trust surface:

1. **Signed challenge/response heartbeats**
	- Add `server_nonce`, `last_server_timestamp`, `heartbeat_counter`, and a per-session `session_key`.
	- Require a keyed MAC over the heartbeat payload so replay, fabrication, and MITM modification are rejected.

2. **Time-based plausibility checks**
	- Track `last_server_receive_time_ms` and a moving tick-rate estimate.
	- Compare `expected_delta` versus `actual_delta` to flag frozen ticks, speedhacks, and burst catch-up behavior.

3. **Decaying risk model**
	- Replace fully linear accumulation with decayed scoring and separate hard/soft risk channels.
	- Keep transient anomalies and hard violations distinct so bursts can decay while persistent abuse escalates.

4. **Server-side behavioral validation**
	- Treat client telemetry as supplementary evidence only.
	- Add server-owned checks for movement plausibility, aim snaps, inventory action timing, CPS bursts, geometry violations, and reaction-time anomalies.

5. **Telemetry entropy and uniformity scoring**
	- Score variance, jitter, and periodicity to catch fake telemetry that looks too perfect.
	- Penalize unrealistically constant heartbeat spacing, latency, and memory-map snapshots.

6. **Session expiration and cleanup**
	- Track `last_activity_ms` and expire stale sessions with bounded cleanup.
	- Prevent slot exhaustion and reduce predictable reuse of dormant state.

7. **Replay detection**
	- Maintain a small duplicate cache of recent heartbeat identifiers and reject replays.
	- Preserve monotonic sequence handling, but do not rely on it as the sole anti-replay control.

8. **Integrity and trust segmentation**
	- Split kernel, usermode, network, and gameplay evidence into separate risk lanes.
	- Promote trust level decisions only when multiple domains corroborate each other.

9. **Thread safety**
	- Guard the session store with mutexes or sharded locks if the native path becomes multi-threaded.
	- Keep score updates, replay caches, and cleanup operations atomic with respect to each session.

10. **Server-issued simulation challenges**
	 - Issue randomized challenge material that requires live simulation or checksum work.
	 - Use the response as challenge evidence, not as the primary source of truth.

11. **Hash hardening**
	 - Replace the current rolling FNV-style integrity value with a keyed, tamper-evident construction for audit state.
	 - Prefer a process-randomized seed or keyed MAC for adversarial inputs.

12. **Trust levels and response policy**
	 - Add trust tiers such as `TRUST_NEW`, `TRUST_ESTABLISHED`, `TRUST_SUSPICIOUS`, `TRUST_RESTRICTED`, and `TRUST_FLAGGED`.
	 - Map policy actions such as warn, challenge, throttle, and quarantine onto those tiers.

### Immediate Baseline-Compatible Follow-Ups

These are the smallest changes that align with the roadmap while preserving the existing O(1) scoring core:

- Add timestamps for receive-time and activity-time tracking.
- Introduce a replay cache for recent heartbeat sequence values.
- Add session timeout cleanup for stale entries.
- Externalize score decay and threshold constants into configurable policy inputs.
- Add audit reconstruction helpers that can re-derive risk decisions from stored snapshots.

## Implementation Todos

- [ ] Wire native anti-cheat wrapper exports into API pipeline
- [ ] Add config-driven score thresholds and decay constants
- [ ] Add replay-time anti-cheat audit reconstruction using stored hashes
- [ ] Add end-to-end test with synthetic suspicious driver burst
- [ ] Add docs for operations response playbook (challenge/quarantine)

# Additional Implementation Todos
- [ ] Add protocol version negotiation support
- [ ] Add structured audit event emitter
- [ ] Add trust-domain segmented scoring model
- [ ] Add entropy/jitter statistical analyzer
- [ ] Add server-side movement plausibility validator
- [ ] Add replay cache eviction strategy
- [ ] Add bounded telemetry rate limiting
- [ ] Add anti-emulation heuristic framework
- [ ] Add distributed session replication strategy
- [ ] Add configurable retention policy for audit telemetry
- [ ] Add deterministic forensic replay tooling
- [ ] Add challenge-response cryptographic validation layer

## Core Architectural Constraint

The anti-cheat system must never become more operationally expensive than the simulation authority it protects.

Detection complexity, telemetry volume, replay retention, and challenge frequency must remain bounded and configurable.

## Non-Goals

This anti-cheat slice does not attempt to:

- provide kernel-level enforcement
- guarantee cheat prevention
- perform invasive system inspection
- collect unrelated personal data
- rely on client-side enforcement as authoritative truth
- prevent all emulator-assisted attackers
- implement DRM-style hardware locking

The primary objective is:
- raising attacker cost
- improving detection confidence
- preserving simulation integrity
- minimizing false positives
- maintaining operational scalability

This is extremely useful later when people try to bolt on invasive features.

## Enforcement Philosophy

Enforcement decisions should prioritize:

1. low false-positive rates
2. corroborated multi-domain evidence
3. statistically persistent anomalies
4. server-authoritative behavioral validation

Permanent or irreversible enforcement actions should avoid relying on:
- isolated telemetry events
- single heartbeat failures
- unsigned client claims
- transient transport instability

This protects against overaggressive policies later.

## Trust Lifecycle

Sessions transition through trust states over time:

- TRUST_NEW
- TRUST_ESTABLISHED
- TRUST_ELEVATED_OBSERVATION
- TRUST_SUSPICIOUS
- TRUST_RESTRICTED
- TRUST_QUARANTINED
- TRUST_FLAGGED

Trust progression should consider:
- session age
- behavioral stability
- challenge success rate
- anomaly persistence
- corroborating evidence

Trust degradation should decay gradually where possible to reduce oscillation.

## Evidence Correlation

High-confidence detection events should prefer correlated anomalies across multiple domains.

Examples:
- timing anomalies + movement impossibility
- replay anomalies + integrity failures
- challenge failures + entropy abnormalities
- suspicious driver telemetry + behavioral impossibility

Single-domain anomalies should generally remain lower-confidence evidence unless explicitly configured otherwise.

## Confidence Scoring

Risk and confidence are tracked independently.

Example:
- high risk + low confidence
- moderate risk + high confidence

Confidence may increase through:
- repeated corroboration
- successful replay validation
- deterministic behavioral impossibility
- repeated challenge failures

This allows policy actions to scale based on detection certainty rather than score magnitude alone.

This is a major architectural improvement.

## Deterministic Simulation Validation

Where feasible, authoritative simulation systems should support deterministic replay validation.

Server-side replay validation may be used to:
- reconstruct suspicious events
- validate impossible state transitions
- verify movement legality
- reproduce enforcement decisions

Replay determinism should remain bounded and operationally configurable.

## Telemetry Sampling

Telemetry collection frequency should support adaptive sampling policies.

Sampling rates may increase during:
- elevated suspicion
- active challenges
- quarantine states
- behavioral anomalies

Sampling rates may decrease for:
- stable trusted sessions
- low-risk environments
- resource-constrained deployments

## Graceful Degradation

Under resource pressure, the anti-cheat pipeline should degrade predictably.

Priority order:
1. authoritative simulation validation
2. replay protection
3. challenge validation
4. behavioral analytics
5. entropy heuristics
6. extended telemetry analysis

Resource exhaustion must not destabilize simulation authority.

## Operational Modes

The pipeline should support configurable modes:

### Observe Mode
- no enforcement
- telemetry collection only

### Soft Enforcement Mode
- warnings
- trust degradation
- challenge escalation

### Active Enforcement Mode
- quarantine
- simulation throttling
- removal actions

### Debug/Test Mode
- synthetic telemetry injection
- deterministic replay testing
- verbose audit emission

## Anti-Tamper Separation

Client anti-tamper systems are treated as telemetry producers rather than enforcement authorities.

Tamper resistance may:
- raise attacker cost
- increase telemetry quality
- slow reverse engineering

Tamper detection alone should not directly authorize irreversible server enforcement.

## Clock Discipline

Server-owned monotonic clocks remain authoritative for:

- replay windows
- heartbeat expiration
- challenge deadlines
- timing anomaly analysis

Client-reported timestamps should be treated as advisory only.

## Secure Configuration

Thresholds, trust policies, challenge settings, and enforcement mappings should support:

- signed configuration deployment
- versioned policy rollout
- staged rollout capability
- rollback support
- audit visibility for policy changes

## Operational Metrics

The pipeline should expose metrics including:

- active session count
- trust-tier distribution
- replay rejection rate
- challenge success/failure rates
- telemetry parse failures
- anomaly rate distributions
- enforcement action frequency
- behavioral validation hit rate

Metrics should support long-horizon tuning and false-positive analysis.

## Synthetic Validation Requirements

The validation framework should support:

- synthetic replay attacks
- malformed heartbeat fuzzing
- telemetry burst simulation
- packet reordering simulation
- timing skew injection
- emulator-style telemetry generation
- challenge replay attacks
- distributed-node synchronization testing

## Policy Isolation

Gameplay tuning and anti-cheat policy tuning should remain independently configurable.

Simulation balance changes should not require anti-cheat threshold modifications unless explicitly intended.

## Future Expansion Boundary

Potential future extensions may include:

- hardware-backed attestation
- secure enclave-assisted integrity validation
- machine-learning-assisted anomaly scoring
- cross-session reputation systems
- distributed trust federation

These remain explicitly outside the scope of this baseline specification.
