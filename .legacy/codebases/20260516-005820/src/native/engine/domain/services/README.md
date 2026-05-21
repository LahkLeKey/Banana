# Engine Domain Services

This folder contains stateless domain services for the engine bounded context.

- `wfc_worldgen_service.*`: WFC-inspired terrain synthesis pass that generates a
	coherent layered heightfield with deterministic neighborhood constraints.
- `cellular_worldgen_service.*`: Cellular automata smoothing pass that denoises
	generated terrain into more traversable regions.