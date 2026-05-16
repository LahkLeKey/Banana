# Spec 050 Drilldown: Native Anti-Cheat Pipeline

## Goal

Define a compute-efficient native anti-cheat path for the server-authoritative multiplayer pipeline using usermode protections and suspicious ring-1/ring-2 class telemetry.

## Scope

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

## Implementation Todos

- [ ] Wire native anti-cheat wrapper exports into API pipeline
- [ ] Add config-driven score thresholds and decay constants
- [ ] Add replay-time anti-cheat audit reconstruction using stored hashes
- [ ] Add end-to-end test with synthetic suspicious driver burst
- [ ] Add docs for operations response playbook (challenge/quarantine)
