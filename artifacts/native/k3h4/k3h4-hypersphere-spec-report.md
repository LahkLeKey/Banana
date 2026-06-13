# K3H4 Refactoring Report

## Executive Summary
The discussion is most coherent when modeled as one pipeline instead of disconnected metaphors.
The pipeline starts with centroidal partitioning (K-means), obtains its geometric interpretation through Voronoi cells, attaches local scale through per-cluster inscribed hypersphere radii, expresses iteration as a fixed-point transform, and enforces correctness through explicit binary contracts (byte order and payload semantics).

K3H4 is used as a compact label for this model:
- K3: three geometric state variables per cluster (center, cell, radius).
- H4: four model blocks (K-means partitioning, hypersphere geometry, harmonic or spectral analysis, hardware or endianness contract).

The critical technical distinction is assignment geometry:
- Multiplicative score s_i(x) = ||x - c_i|| / r_i induces multiplicatively weighted Voronoi behavior.
- Power score p_i(x) = ||x - c_i||^2 - r_i^2 induces a power diagram.

These are related but not equivalent. Multiplicative weighting can produce non-convex or disconnected regions; power cells are polyhedral with hyperplane boundaries.

## Conversation Recap as a Unified Map

| Conversation concept | Role in K3H4 |
| --- | --- |
| Lambda calculus | Fixed-point view of iterative clustering |
| K-means | Baseline assignment and center update engine |
| Voronoi cells | Geometry of nearest-center assignment |
| Inscribed hyperspheres | Cluster-local scale via radius r_i |
| Endianness | Representation contract before geometry exists |
| Harmonic or spectral view | Graph-eigenmode layer for non-Euclidean structure |

This map is operational, not mnemonic. In standard Lloyd-style K-means, nearest-center assignment is exactly an ordinary Voronoi partition. Once radius enters assignment, the geometry changes and must be specified explicitly.

## Mathematical Core

### 1) Baseline K-means and Voronoi Partition
Given centers c_1,...,c_k in R^d, assignment is

A(x) = argmin_i ||x - c_i||.

This defines ordinary Voronoi cells V_i. If each cell uses a centered insphere radius, the maximal centered radius is

r_i = 0.5 * min_{j != i} ||c_i - c_j||

for the unweighted Voronoi configuration.

### 2) Radius-Aware Assignment Variants
Two common radius-aware scores are:

s_i(x) = ||x - c_i|| / r_i

p_i(x) = ||x - c_i||^2 - r_i^2

Interpretation:
- Minimizing s_i corresponds to multiplicative weighting.
- Minimizing p_i corresponds to additive power distance.

Practical consequence:
- Use multiplicative weighting when strict scale normalization is desired and non-convex geometry is acceptable.
- Use power diagrams when convex polyhedral cells and stable point-location behavior are preferred.

### 3) Fixed-Point Form
Let F map (C, R, A) to updated (C', R', A') by:
1. Assignment using selected score family.
2. Center update by cluster mean.
3. Radius closure from updated centers.

The loop seeks a fixed point:

(C, R, A) = F(C, R, A).

This is the lambda-calculus style reading of an iterative K-means pipeline: a recursive operator converging to a stable state under deterministic tie-break and stopping rules.

## K3H4 State Model

Per-cluster geometric state:

(c_i, V_i, r_i), i = 1,...,k.

System-level state can be summarized as:

Sigma = (B, H, X, W, Z, C, R, A)

where:
- B is raw bytes.
- H is decode and hardware contract (dtype, byte order, alignment, device).
- X is decoded feature matrix.
- W is optional affinity graph.
- Z is embedding (Z = X when spectral layer is bypassed).
- C are centers, R are radii, A is assignment map.

Decode precedence is strict:

X = Decode_H(B).

If decode is wrong, geometry is undefined regardless of clustering algorithm quality.

## Harmonic Layer: Rigorous Use
The term harmonic is made precise through spectral clustering.
When data are graph-like, manifold-shaped, or non-convex in ambient space, construct a graph Laplacian and embed first:

L = D - W, or L_sym = I - D^(-1/2) W D^(-1/2).

Then run the K3H4 clustering stage in the eigenvector embedding Z. This replaces informal harmonic analogy with a mathematically controlled upstream transformation.

## Endianness and Interchange Contract
Endianness is not a geometric parameter; it is a representation parameter.
It must be explicit in all binary interchange and ABI boundaries.

Operational rule:
- Byte-to-number decoding is part of model validity, not an implementation afterthought.

Implementation-aligned safeguards:
- Versioned envelope.
- Canonical byte order declaration.
- Payload length validation.
- Deterministic error states for malformed, truncated, or unsupported payloads.
- Optional CRC or equivalent integrity check.

## Banana Mapping

### Native
- Native orchestration entry remains under src/native/k3h4.
- Runtime emits deterministic K3H4 outputs and envelope metadata.

### API
- API remains the orchestration and contract boundary.
- FFI decode maps envelope fields to stable response contracts.

### Frontend
- Frontend remains presentation-only for production metrics.
- No production local recomputation fallback.

This preserves compute authority in native, contract authority in API, and render authority in frontend.

## Recommended Refactoring Path
1. Choose assignment family explicitly (multiplicative vs power) and document rationale.
2. Define deterministic tie-break rules and convergence criteria.
3. Close radii after center updates each iteration.
4. Keep spectral embedding optional but first-class for non-Euclidean regimes.
5. Treat binary decode policy as a first-class part of correctness proofs and tests.

## Final Statement
K3H4 is most useful as a layered contract between geometry and systems:
- Geometry defines the intended partition and scale behavior.
- Fixed-point iteration defines the update logic.
- Spectral preprocessing extends applicability when Euclidean partitions are insufficient.
- Endianness and ABI contracts ensure all participants compute on the same numbers.

Without the last layer, the first three are mathematically correct but operationally fragile.
