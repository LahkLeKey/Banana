# K3H4 Notes: Lambda-Style K-Means, Hyperspheres, Endianness, Harmonics

This note captures the conceptual model requested in issue 1157 and ties it to the current K3H4 runtime contract.

## 1. Lambda-style view of K-means

K-means alternates two transformations over a centroid set C:

1. Assign each vector x to its nearest centroid.
2. Recompute each centroid as the mean of vectors assigned to it.

In operator form:

- C(t+1) = K(C(t))

At convergence, centroids are a fixed point of K:

- C* = K(C*)

That fixed-point perspective is the cleanest bridge to lambda-calculus style reasoning: repeated function application until an invariant state is reached.

## 2. Adding inscribed hyperspheres

Standard K-means stores one point per cluster (the centroid). The hypersphere extension stores a pair:

- (c(i), r(i))

where r(i) is the inradius of the cluster's Voronoi cell, approximated by nearest-center separation:

- r(i) = 0.5 * min over j != i of ||c(i) - c(j)||

This adds a local scale/separation signal. In Banana K3H4, that concept is represented by per-cluster radius fields in the runtime output:

- RuntimeNetcodeK3h4Radius.nearest_neighbor_distance_q16
- RuntimeNetcodeK3h4Radius.inscribed_radius_q16

See [modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_metrics.h](modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_metrics.h).

## 3. Weighted assignment with radius awareness

A radius-aware score normalizes distance by cluster scale:

- s(i, x) = ||x - c(i)|| / r(i)

Assignment chooses the minimum score cluster. This is closely related to weighted Voronoi or power-style families.

In Banana K3H4, assignment family is explicitly modeled:

- RUNTIME_NETCODE_K3H4_ASSIGNMENT_MULTIPLICATIVE
- RUNTIME_NETCODE_K3H4_ASSIGNMENT_POWER

and the selected mode is used to produce weighted score outputs in RuntimeNetcodeWeightedVoronoiScore.

See [modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_metrics.h](modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_metrics.h).

## 4. Endianness: not geometry, but transport correctness

Endianness does not change clustering math. It changes how bytes are interpreted as numbers. If encoding/decoding conventions disagree, centroids and distances become invalid.

In Banana K3H4, this is first-class in the export/ABI envelope:

- byte_order_tag is written and validated.
- Endianness decode path is tracked in observability.

Relevant locations:

- [modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_export.c](modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_export.c)
- [modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_metrics.h](modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_metrics.h)

## 5. Harmonic interpretation

Endian order itself is not a harmonic quantity. But if a byte sequence is viewed as a sampled signal, reversing order preserves magnitude spectrum and changes phase. That provides a useful analogy:

- Representation conventions alter phase-level interpretation.
- Stable geometric structure should survive valid representation changes.

For clustering, a practical harmonic bridge is spectral methods over affinity graphs. Banana K3H4 models this as a selectable spectral mode:

- RUNTIME_NETCODE_K3H4_SPECTRAL_DISABLED
- RUNTIME_NETCODE_K3H4_SPECTRAL_AFFINITY_GRAPH

This keeps the conceptual link to manifold harmonics while preserving deterministic runtime contracts.

## 6. Domain summary

- Lambda-style framing: iterative operator to fixed point.
- Hypersphere framing: cluster center plus separation radius.
- Endianness framing: transport/ABI integrity, not geometry.
- Harmonic framing: useful analogy for representation and spectral structure.

Together, these are complementary views of one K3H4 system rather than separate algorithms.

## 7. Concept-to-contract mapping

Use this map when reviewing runtime output or ABI traces.

| Concept | Runtime contract surface | Why it matters |
|---|---|---|
| Fixed-point convergence | `RuntimeNetcodeK3h4Observability.convergence_status`, `iteration_count` | Shows whether repeated application of the operator reached a stable state. |
| Hypersphere separation | `RuntimeNetcodeK3h4Radius.nearest_neighbor_distance_q16`, `inscribed_radius_q16` | Captures cluster separation and local scale for robust assignment. |
| Radius-aware assignment | `RuntimeNetcodeWeightedVoronoiScore.distance_to_center_q16`, `weighted_score_q16`, `score_validity` | Encodes normalized or power-style cluster selection behavior. |
| Spectral structure | `RuntimeNetcodeSpectralProxy.frequency_proxy_q16`, `amplitude_proxy_q16`, `spectral_state` | Provides a compact harmonic/spectral readout tied to cluster geometry. |
| Transport integrity | Envelope `byte_order_tag`, `payload_crc32`, observability decode path | Prevents representation mismatches from silently corrupting geometry. |

Primary reference: [modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_metrics.h](modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_metrics.h).

## 8. Local verification checklist

Use this short checklist before changing K3H4 clustering or export behavior.

1. Build native target and run focused K3H4 tests.
2. Confirm output envelope fields are stable (`contract_version`, `byte_order_tag`, CRC fields).
3. Validate radius and weighted-score arrays are populated and deterministic for identical input.
4. Confirm observability fields are present and internally consistent (`convergence_status`, `iteration_count`, decode path).
5. If assignment family or spectral mode changes, verify both mode enums remain backward compatible.

Helpful entry points:

- [src/native/include/banana_native_v3.h](src/native/include/banana_native_v3.h)
- [src/native/scaffold/native_entry.c](src/native/scaffold/native_entry.c)
- [tests/native/CMakeLists.txt](tests/native/CMakeLists.txt)