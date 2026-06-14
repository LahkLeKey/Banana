# K3H4 Refactoring Report

## Executive Summary
The discussion is most coherent when modeled as one pipeline instead of disconnected metaphors. The pipeline starts with centroidal partitioning (K-means), obtains its geometric interpretation through Voronoi cells, attaches local scale through per-cluster inscribed hypersphere radii, expresses iteration as a fixed-point transform, and enforces correctness through explicit binary contracts (byte order and payload semantics).

K3H4 is used as a compact label for this model:

- K3: three geometric state variables per cluster (center, cell, radius).
- H4: four model blocks (K-means partitioning, hypersphere geometry, harmonic or spectral analysis, hardware or endianness contract).

The critical technical distinction is assignment geometry:

- Multiplicative score $s_i(x) = ||x - c_i|| / r_i$ induces multiplicatively weighted Voronoi behavior.
- Power score $p_i(x) = ||x - c_i||^2 - r_i^2$ induces a power diagram.

These are related but not equivalent. Multiplicative weighting can produce non-convex or disconnected regions; power cells are polyhedral with hyperplane boundaries.

## Conversation Recap As A Unified Map

| Conversation concept | Role in K3H4 |
| --- | --- |
| Lambda calculus | Fixed-point view of iterative clustering |
| K-means | Baseline assignment and center update engine |
| Voronoi cells | Geometry of nearest-center assignment |
| Inscribed hyperspheres | Cluster-local scale via radius $r_i$ |
| Endianness | Representation contract before geometry exists |
| Harmonic or spectral view | Graph-eigenmode layer for non-Euclidean structure |

This map is operational, not mnemonic. In standard Lloyd-style K-means, nearest-center assignment is exactly an ordinary Voronoi partition. Once radius enters assignment, the geometry changes and must be specified explicitly.

## Mathematical Core

### 1) Baseline K-means and Voronoi Partition
Given centers $c_1,...,c_k$ in $\mathbb{R}^d$, assignment is

$$
A(x) = \arg\min_i ||x - c_i||.
$$

This defines ordinary Voronoi cells $V_i$. If each cell uses a centered insphere radius, the maximal centered radius is

$$
r_i = 0.5 * \min_{j \neq i} ||c_i - c_j||
$$

for the unweighted Voronoi configuration.

### 2) Radius-Aware Assignment Variants
Two common radius-aware scores are:

$$
s_i(x) = ||x - c_i|| / r_i
$$

$$
p_i(x) = ||x - c_i||^2 - r_i^2
$$

Interpretation:

- Minimizing $s_i$ corresponds to multiplicative weighting.
- Minimizing $p_i$ corresponds to additive power distance.

Practical consequence:

- Use multiplicative weighting when strict scale normalization is desired and non-convex geometry is acceptable.
- Use power diagrams when convex polyhedral cells and stable point-location behavior are preferred.

### 3) Fixed-Point Form
Let $F$ map $(C, R, A)$ to updated $(C', R', A')$ by:

1. Assignment using selected score family.
2. Center update by cluster mean.
3. Radius closure from updated centers.

The loop seeks a fixed point:

$$
(C, R, A) = F(C, R, A).
$$

This is the lambda-calculus style reading of an iterative K-means pipeline: a recursive operator converging to a stable state under deterministic tie-break and stopping rules.

## K3H4 State Model
Per-cluster geometric state:

$$
(c_i, V_i, r_i), i = 1,...,k.
$$

System-level state can be summarized as:

$$
\Sigma = (B, H, X, W, Z, C, R, A)
$$

where:

- $B$ is raw bytes.
- $H$ is decode and hardware contract (dtype, byte order, alignment, device).
- $X$ is decoded feature matrix.
- $W$ is optional affinity graph.
- $Z$ is embedding ($Z = X$ when spectral layer is bypassed).
- $C$ are centers, $R$ are radii, $A$ is assignment map.

Decode precedence is strict:

$$
X = Decode_H(B).
$$

If decode is wrong, geometry is undefined regardless of clustering algorithm quality.

## Harmonic Layer: Rigorous Use
The term harmonic is made precise through spectral clustering. When data are graph-like, manifold-shaped, or non-convex in ambient space, construct a graph Laplacian and embed first:

$$
L = D - W, \quad \text{or} \quad L_{sym} = I - D^{-1/2} W D^{-1/2}.
$$

Then run the K3H4 clustering stage in the eigenvector embedding $Z$. This replaces informal harmonic analogy with a mathematically controlled upstream transformation.

## Algorithms, Complexity, And Variant Comparison

For fixed radii, the radius-aware K3H4 objective remains Lloyd-like: the assignment step minimizes a mode-specific score and the centroid update remains the arithmetic mean because the radius is constant within a fixed assignment/update substep. The radius closure step is what turns the method from pure coordinate descent into a fixed-point geometric closure.

The update equations are therefore:

$$
c_i^{(t+1)} = \frac{1}{|S_i^{(t)}|}\sum_{z_t \in S_i^{(t)}} z_t
$$

$$
r_i^{(t+1)} = \frac{1}{2}\min_{j \neq i} ||c_i^{(t+1)} - c_j^{(t+1)}||
$$

optionally stabilized by damping:

$$
r_i^{(t+1)} = \max\left(\varepsilon, \alpha r_i^{(t)} + (1-\alpha) \frac{1}{2}\min_{j \neq i} ||c_i^{(t+1)} - c_j^{(t+1)}||\right), \quad 0 \leq \alpha < 1
$$

The multiplicative weighted assignment rule is:

$$
s_i(x) = \frac{||x-c_i||}{r_i}, \quad A(x) = \arg\min_i s_i(x)
$$

which defines multiplicatively weighted Voronoi cells:

$$
V_i = \left\{x : \frac{||x-c_i||}{r_i} \leq \frac{||x-c_j||}{r_j}, \forall j \right\}
$$

Their boundaries are Apollonian spheres when $r_i \neq r_j$ and hyperplanes when $r_i = r_j$. The additive analogue is the power-diagram score:

$$
p_i(x) = ||x-c_i||^2 - r_i^2
$$

Power diagrams are usually the better choice when convex cells, clean point location, and stable geometric infrastructure matter more than multiplicative scale normalization.

### Fixed-Point Sketch

Using lambda-calculus style notation with tuples and conditionals:

$$
Y \equiv \lambda f.(\lambda x.f(xx))(\lambda x.f(xx))
$$

$$
Assign \equiv \lambda C.\lambda R.\lambda Z.\, map\,(\lambda z.\arg\min_i\,(norm(z,c_i)/\max(r_i,\varepsilon)))\, Z
$$

$$
UpdateC \equiv \lambda A.\lambda Z.\, map\,(\lambda i. mean(filter_i\,A\,Z))\, I_k
$$

$$
UpdateR \equiv \lambda C.\, map\,(\lambda i. 0.5 * \min_{j\neq i} norm(c_i,c_j))\, I_k
$$

$$
Embed \equiv \lambda X.\, if\ spectral\ then\ EigEmbed(X)\ else\ X
$$

$$
Step \equiv \lambda \Sigma.\, let\ X = Decode_H(Bytes\,\Sigma)\ in\ let\ Z = Embed\,X\ in\ let\ A = Assign\,(C\,\Sigma)\,(R\,\Sigma)\,Z\ in\ let\ C' = UpdateC\,A\,Z\ in\ let\ R' = UpdateR\,C'\ in\ \langle Bytes\,\Sigma, H\,\Sigma, X, W(X), Z, C', R', A \rangle
$$

$$
K3H4 \equiv Y\,(\lambda self.\lambda \Sigma.\, if\ Converged\,\Sigma\ then\ \Sigma\ else\ self\,(Step\,\Sigma))
$$

### Practical Pseudocode

```python
def k3h4(byte_buffer, hardware_contract, k, spectral=False,
		 max_iter=100, tol=1e-5, eps=1e-8, damping=0.5):
	X = decode(byte_buffer, hardware_contract)
	Z = spectral_embed(X, k) if spectral else X
	C = init_centroids(Z, k)
	R = init_radii_from_centroids(C, eps)

	for _ in range(max_iter):
		A = argmin_over_i(lambda z, i: norm(z - C[i]) / max(R[i], eps), Z)
		C_new = [
			mean(Z[A == i]) if any(A == i) else reinit_center(Z, C, A, i)
			for i in range(k)
		]
		R_geo = [
			0.5 * min(norm(C_new[i] - C_new[j]) for j in range(k) if j != i)
			for i in range(k)
		]
		R_new = [
			max(eps, damping * R[i] + (1 - damping) * R_geo[i])
			for i in range(k)
		]

		if max(norm(C_new[i] - C[i]) for i in range(k)) < tol and \
		   max(abs(R_new[i] - R[i]) for i in range(k)) < tol:
			C, R = C_new, R_new
			break

		C, R = C_new, R_new

	return C, R, A
```

### Complexity Notes

- Ordinary centroidal clustering still spends most time in nearest-center assignment and centroid recomputation.
- The K3H4 radius closure adds an all-pairs centroid-distance term, naturally $O(k^2 d)$ per iteration.
- With spectral embedding enabled, the dominant cost often shifts to graph construction and eigensolution.
- Exact multiplicatively weighted Voronoi materialization is usually unnecessary and can be much more expensive than score-based use.

### Variant Summary

| Variant | Representation | Assignment rule | Update rule | Geometry | Practical tradeoff |
| --- | --- | --- | --- | --- | --- |
| Standard K-means | Centroids | nearest center | mean of assigned points | ordinary Voronoi | strong baseline for compact roughly isotropic clusters |
| Multiplicative K3H4 | centroids + radii | $||x-c_i|| / r_i$ | mean + radius closure | multiplicatively weighted Voronoi | strong local scale normalization, more complex geometry |
| Power K3H4 | centroids + radii | $||x-c_i||^2 - r_i^2$ | mean + radius closure | power diagram | convex cells and simpler bisectors |
| Spectral K3H4 | graph + embedding + centroids + radii | mode-specific score in eigenvector space | graph embedding + centroid/radius updates | harmonic graph geometry + local scale | better for non-convex or manifold-shaped data |

A rigorous caution still applies: K3H4 should be described as a fixed-point geometric iteration, not as a single clean convex optimization program, because the radius closure step is a geometric refactoring rule rather than the exact minimizer of one standard smooth objective.

## Systems And Numerical Implementation Notes

The hardware layer is not incidental. It decides whether the bytes on disk or the wire become the intended numeric coordinates before any distances, radii, Voronoi cells, or eigenvectors are computed.

Implementation rules:

- Decode explicitly.
- Normalize once.
- Only then cluster.
- Treat byte order, dtype, and alignment as required input declarations.

Practical guidance:

- Keep feature matrices, centroids, and radii contiguous in memory.
- Separate hot numeric kernels from serialization paths.
- Prefer memory layouts that allow adjacent scalar/vector lane access on CPU and coalesced access patterns on GPU.

Recommended endianness tests:

1. Maintain golden little-endian and big-endian fixtures.
2. Include sentinels such as negative values, infinities, and NaNs.
3. Test both reinterpret-only and byteswap-plus-reinterpret paths.
4. Verify cross-platform reproducibility after explicit decode and before clustering.

## Harmonic And Spectral Interpretation

There are two useful senses of harmonic structure here.

### Heuristic Local Harmonic Lens

Given cluster radius $r_i$, a characteristic frequency scale can be attached as:

$$
\omega_i = \kappa / (r_i + \varepsilon), \quad f_i = \omega_i / 2\pi
$$

This is an interpretation device, not a theorem about K-means. Smaller inscribed spheres imply higher characteristic frequencies; larger ones imply lower frequencies.

### Rigorous Global Harmonic Lens

The mathematically rigorous harmonic object is the graph Laplacian. Spectral clustering builds a similarity graph, computes low-frequency eigenvectors, and clusters the row vectors of that embedding. The combined interpretation is:

- local geometry and scale $\leftrightarrow r_i$
- global harmonic structure $\leftrightarrow$ low-eigenvalue structure of $L$

That is why spectral-first K3H4 is often preferable on manifold or graph-structured data: the embedding resolves the global organization first, then the centroid-plus-radius layer captures local scale in that space.

## Recommended Experiments And Open Questions

### Experimental Program

Compare:

1. standard K-means
2. multiplicative K3H4
3. power-mode K3H4
4. spectral clustering
5. Gaussian mixture models

Run them on:

- isotropic blobs
- unequal-size or unequal-variance blobs
- anisotropic Gaussians
- non-convex manifolds such as rings or spirals

Evaluate with:

- weighted distortion / inertia
- silhouette score
- Davies-Bouldin
- Calinski-Harabasz
- adjusted Rand or adjusted mutual information when labels exist
- seed stability
- platform stability
- decode reproducibility under explicit byte-order variation

### Main Recommendation

Adopt two K3H4 modes, not one.

- Use `multiplicative` when you want normalized distance in units of local cluster radius.
- Use `power` when you want convex weighted cells, simpler bisectors, and more stable geometric infrastructure.
- Use the spectral layer only when the affinity graph carries information that plain Euclidean cells do not.
- Regardless of mode, force the hardware layer to declare byte order, dtype, and alignment explicitly before clustering begins.

### Open Questions

1. Can one derive a single joint objective whose block updates include assignment, centroid means, and radius closure without changing the qualitative model?
2. Should radius updates depend jointly on centroid spacing and local graph conductance when spectral structure is enabled?
3. Can multiplicative and power variants be blended adaptively per cluster pair instead of globally?
4. Which parts of the K3H4 pipeline should be required to be bitwise stable across hardware, and which only numerically equivalent?

## Endianness And Interchange Contract
Endianness is not a geometric parameter; it is a representation parameter. It must be explicit in all binary interchange and ABI boundaries.

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

- Native orchestration entry remains under `src/native/k3h4`.
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

## Sources

- MacQueen and Lloyd-style centroidal clustering literature for within-cluster variance minimization.
- Aurenhammer on Voronoi and power diagrams.
- Luxburg, Shi, and Malik on spectral clustering and graph Laplacians.
- Python `struct`, NumPy byte swapping, XDR, Arrow, and Parquet references for binary representation discipline.
- Intel and NVIDIA guidance on alignment, vectorization, and coalesced access for practical numeric kernels.