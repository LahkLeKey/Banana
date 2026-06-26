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