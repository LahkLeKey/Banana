# Quickstart: Native Hypersphere K-Means Analytics

## 1) Prerequisites
- Branch: `035-native-hypersphere-kmeans`
- Required env:
  - `BANANA_NATIVE_PATH` points to compiled native library.
  - `BANANA_NETCODE_ADAPTER=ffi`.
  - `BANANA_NETCODE_HYPERSPHERE_KMEANS_ENABLED=true` for staged feature validation.
  - `BANANA_NETCODE_HYPERSPHERE_KMEANS_COHORT=<cohort-name>` (for staged cohorts, default `all`).

### Rollout Matrix

| Mode | `BANANA_NETCODE_HYPERSPHERE_KMEANS_ENABLED` | `BANANA_NETCODE_HYPERSPHERE_KMEANS_COHORT` | Expected `/api/netcode/analytics` behavior |
|------|----------------------------------------------|---------------------------------------------|--------------------------------------------|
| Legacy fallback | `false` | any | HTTP `503` with rollout-disabled contract |
| Internal validation | `true` | `internal` | Full analytics payload with rollout metadata |
| Cohort staged | `true` | `<cohort>` | Full analytics payload with cohort marker |
| General rollout | `true` | `all` | Full analytics payload for all callers |

## 2) Build native runtime
```bash
cmake -S src/native -B out/v3-native
cmake --build out/v3-native
```

### Optional high-performance backend knobs
- `BANANA_NETCODE_ENABLE_BLAS=ON|OFF` (CMake option, default `ON`): enables CBLAS path when `cblas.h` and BLAS libs are available.
- `BANANA_NETCODE_KMEANS_BACKEND=auto|blas|scalar` (runtime env): selects distance kernel backend. `auto` prefers BLAS when compiled in; otherwise deterministic scalar path.
- CUDA/OpenCL/Eigen integration can be layered behind the same backend selector once toolchain/profile support is added for the target environment.

## 3) Run focused native tests
```bash
ctest -C Debug --test-dir out/v3-native -R "netcode|hypersphere|kmeans" --output-on-failure
```

## 4) Run API route/service contract tests
```bash
cd src/typescript/api
bun test src/routes/netcode.contract.test.ts
bun test src/services/nativeNetcode*.test.ts
```

## 5) Run React consumer tests (presentation-only assertions)
```bash
cd src/typescript/react
bun test src/domain/notebook/**/*.test.ts
```

## 6) Smoke API orchestration path
```bash
cd /c/Github/Banana
bun -e "import { registerNetcodeRoutes } from './src/typescript/api/src/routes/netcode.ts'; console.log(typeof registerNetcodeRoutes);"
```

## 7) Determinism verification loop
1. Submit the same `/api/netcode/analytics` request 20 times.
2. Collect response bodies.
3. Assert byte-identical JSON after canonical key order normalization.
4. Assert `observability.deterministicHash` is identical for all runs.

## 8) Rollback drill
1. Set `BANANA_NETCODE_HYPERSPHERE_KMEANS_ENABLED=false`.
2. Optionally set `BANANA_NETCODE_HYPERSPHERE_KMEANS_COHORT=rollback` for evidence tagging.
3. Re-run API contract test for `/api/netcode/analytics`.
4. Verify legacy payload path is preserved and UI shows unavailable state for K-means panel.
