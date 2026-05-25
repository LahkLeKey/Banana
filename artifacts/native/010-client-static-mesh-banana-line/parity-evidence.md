# 010 Client Static Mesh Parity Evidence

Date: 2026-05-25

## Validation

Command:
- `cmake --build out/v3-native --target banana_runtime_terrain_static_mesh_generation_test && C:/Github/Banana/out/v3-native/engine/Debug/banana_runtime_terrain_static_mesh_generation_test.exe`

Result:
- `STATIC_MESH_PARITY_PASS`
- `STATIC_MESH_AND_ROUTE_PARITY_PASS`
- `STATIC_MESH_ROUTE_ENVELOPE_PASS`
- `STATIC_MESH_TYPED_SUBDOMAIN_PASS`

## Coverage Notes

- Verified deterministic parity for every canonical Banana Archipelago region profile.
- Verified sampled local chunk offsets for each profile.
- Verified county anchor generation remains consistent with the regional hub baseline.
- Verified named Banana Line corridor metadata still resolves after parity expansion.
- Verified deterministic parity for each named Banana Line corridor and adjacent corridor leg.
- Verified route lengths remain inside the playtest envelope ceiling for direct and named Banana Line routes.
- Verified the static-mesh implementation now lives in a typed subdomain folder under `src/native/engine/runtime/terrain/static_mesh/`.

## End-to-End Bundle

- See `artifacts/native/010-client-static-mesh-banana-line/end-to-end-evidence-bundle.md` for combined native and API merge-readiness evidence.
