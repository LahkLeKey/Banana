# Banana Native V3
Pure C gameplay/runtime scaffold.

## Netcode Hypersphere Architecture
The hypersphere netcode path is split into a thin facade plus focused orchestration modules so server-authoritative compute stays deterministic and ABI-safe.

- `runtime/netcode/hypersphere/netcode_hypersphere.c`: public facade entrypoint.
- `runtime/netcode/hypersphere/netcode_hypersphere_pipeline.c`: orchestration sequence only.
- `runtime/netcode/hypersphere/netcode_hypersphere_pipeline_setup.c`: input validation and context initialization.
- `runtime/netcode/hypersphere/netcode_hypersphere_geometry.c`: normalization, projection nodes, and summary geometry metrics.
- `runtime/netcode/hypersphere/netcode_hypersphere_cluster_scoring.c`: centers, radii, spectral proxies, and weighted Voronoi scoring.
- `runtime/netcode/hypersphere/netcode_hypersphere_observability.c`: observability and deterministic hash publication.

This keeps the dependency direction explicit: facade -> pipeline -> focused compute modules, while ABI envelope encoding remains outside raw hypersphere compute in the netcode ABI layer.
