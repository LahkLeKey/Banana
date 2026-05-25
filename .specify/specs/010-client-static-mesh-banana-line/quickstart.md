# Quickstart: Client Static Mesh and Banana Line Playtest

## Native build and playtest loop

1. Configure once:
   cmake -S src/native -B out/v3-native

2. Build static mesh playtest target:
   cmake --build out/v3-native --target banana_runtime_terrain_static_mesh_generation_test

3. Run deterministic generation and Banana Line route playtest:
   C:/Github/Banana/out/v3-native/engine/Debug/banana_runtime_terrain_static_mesh_generation_test.exe

4. Review generated evidence:
   artifacts/native/010-client-static-mesh-banana-line/parity-evidence.md

## Incremental generator workflow

1. Add or adjust one profile or corridor in src/native/engine/runtime/terrain/static_mesh/terrain_static_mesh_domain.c.
2. Rebuild only banana_runtime_terrain_static_mesh_generation_test.
3. Run the test executable and capture stdout/stderr in artifacts/native for evidence.
4. Repeat for the next profile or route corridor after adjusting the native scaffold.

## Follow-up server storage gate workflow

1. Keep baseline mesh payloads client-side only.
2. Persist only profile IDs, route IDs, procedural contracts, deltas, and progression ledgers server-side.
3. Track total DB usage and keep terrain+ledger footprint within the 10 GB system cap.
