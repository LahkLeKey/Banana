# Banana Native V3
Pure C gameplay/runtime scaffold.

## Netcode Hypersphere Architecture
The metrics netcode path is split into a thin facade plus focused orchestration modules so server-authoritative compute stays deterministic and ABI-safe.

- `runtime/netcode/k3h4/netcode_k3h4.c`: public facade entrypoint.
- `runtime/netcode/k3h4/netcode_k3h4_pipeline.c`: orchestration sequence only.
- `runtime/netcode/k3h4/netcode_k3h4_pipeline_setup.c`: input validation and context initialization.
- `runtime/netcode/k3h4/netcode_k3h4_geometry.c`: normalization, projection nodes, and summary geometry metrics.
- `runtime/netcode/k3h4/netcode_k3h4_cluster_scoring.c`: centers, radii, spectral proxies, and weighted Voronoi scoring.
- `runtime/netcode/k3h4/netcode_k3h4_observability.c`: observability and deterministic hash publication.

This keeps the dependency direction explicit: facade -> pipeline -> focused compute modules, while ABI envelope encoding remains outside raw metrics compute in the netcode ABI layer.
