# Research: Native Hypersphere K-Means Analytics

## Decision 1: Deterministic fixed-point K-means core in native runtime
- Decision: Implement K-means compute in C using fixed-point arithmetic (`Q16.16`) for centers, distances, and convergence checks.
- Rationale: Fixed-point eliminates architecture-dependent floating-point drift and supports byte-identical replay across runtime channels.
- Alternatives considered: Float32-only compute (rejected due to non-deterministic rounding across compilers/targets); Float64 compute (rejected for ABI payload cost and unnecessary precision for this workload).

## Decision 2: Stable assignment and ordering policy
- Decision: Use deterministic tie-break rules (lowest cluster index on equal distance) and stable cluster ordering by first-seen input index then centroid lexicographic order.
- Rationale: Reproducible output requires deterministic ordering beyond mathematical equivalence.
- Alternatives considered: Hash-map or insertion-order accumulation (rejected because order can differ by run/allocator behavior).

## Decision 3: Inscribed hypersphere radius derivation with guardrails
- Decision: Radius per cluster is `0.5 * min(distance(center_i, center_j))` with explicit sentinel behavior for single-cluster output and minimum-radius clamp for near-duplicate centers.
- Rationale: Satisfies FR-003 while preventing divide-by-zero in weighted scoring.
- Alternatives considered: Global average radius (rejected, violates requirement); epsilon added only at score stage (rejected, hides invalid radius state).

## Decision 4: Weighted Voronoi score and spectral proxy contract
- Decision: Compute weighted score as `distance_to_center / max(radius, radius_floor)` and expose `spectral_frequency_proxy = spectral_scale / max(radius, radius_floor)` plus validity metadata.
- Rationale: Meets FR-004 and FR-005 with deterministic failure-state visibility.
- Alternatives considered: Unweighted Voronoi distance (rejected, misses normalization requirement); FFT-derived spectrum (rejected as out-of-scope and non-minimal).

## Decision 5: Endianness-safe ABI envelope with explicit versioning
- Decision: Introduce a versioned binary envelope with canonical little-endian encoding and explicit decode status codes for unsupported versions and malformed payloads.
- Rationale: Guarantees cross-boundary stability and testable compatibility behavior (FR-008, FR-009, FR-010).
- Alternatives considered: Host-endian structs over FFI only (rejected for mixed-endian fixture support); JSON-only boundary (rejected for native ABI throughput/size constraints).

## Decision 6: API-only production compute and React presentation-only behavior
- Decision: Perform production compute through API orchestration backed by native FFI; React consumes server-provided metrics and may only run optional non-production preview math under explicit debug mode.
- Rationale: Enforces FR-006 and FR-007 while preserving client consistency.
- Alternatives considered: Client fallback production compute (rejected due to determinism drift and policy mismatch).

## Decision 7: Rollout strategy and rollback controls
- Decision: Gate new analytics contract behind staged flags (`BANANA_NETCODE_K3H4_ENABLED`, cohort routing in API), with hard rollback to existing analytics payload shape.
- Rationale: Supports FR-013 and SC-005 with low-risk deployment.
- Alternatives considered: Big-bang rollout (rejected for operational risk).

## Decision 8: Validation matrix across native/API/React layers
- Decision: Add deterministic native tests (repeatability, radius edge cases, score validity), API contract tests (version + endianness fixtures + failure states), and React consumer tests asserting no production local recompute.
- Rationale: Directly satisfies FR-014 and constitution quality gate requirements.
- Alternatives considered: API-only tests (rejected due to insufficient native/consumer coverage).
