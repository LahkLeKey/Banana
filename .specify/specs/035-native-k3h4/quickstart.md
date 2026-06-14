# Quickstart: Native K3H4 Analytics

## 1) Prerequisites
- Branch: `035-native-k3h4`
- Required env:
  - `BANANA_NATIVE_PATH` points to compiled native library.
  - `BANANA_NETCODE_ADAPTER=ffi`.
  - `BANANA_NETCODE_K3H4_ENABLED=true` for staged feature validation.
  - `BANANA_NETCODE_K3H4_COHORT=<cohort-name>` (for staged cohorts, default `all`).

### Explicit Runtime Controls

Request-level controls for `/api/netcode/analytics`, `/api/netcode/vector`, and `/api/netcode/k3h4`:

- `k3h4Mode`: `multiplicative | power`
- `spectralMode`: `disabled | affinity-graph`

If omitted, defaults are:

- `k3h4Mode=multiplicative`
- `spectralMode=disabled`

### Native Hardware Preflight Contract

Before native K3H4 clustering executes, preflight declarations must match:

- `hardwareByteOrderTag=0x01020304`
- `hardwareDtypeTag=1` (`f32/q16 mixed`)
- `hardwareAlignmentBytes=4`

Mismatches fail fast with deterministic invalid-payload contract status.

### Rollout Matrix

| Mode | `BANANA_NETCODE_K3H4_ENABLED` | `BANANA_NETCODE_K3H4_COHORT` | Expected `/api/netcode/analytics` behavior |
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
- `BANANA_NETCODE_K3H4_BACKEND=auto|blas|scalar` (runtime env): selects distance kernel backend. `auto` prefers BLAS when compiled in; otherwise deterministic scalar path.
- Production contract is CPU-only (scalar/BLAS). GPU backends are intentionally out of scope for this feature.

## 3) Run focused native tests
```bash
ctest -C Debug --test-dir out/v3-native -R "netcode|k3h4" --output-on-failure
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

Targeted US3 contract checks:

```bash
cd src/typescript/react
bun test src/domain/notebook/useNetcodeSession.test.ts src/domain/notebook/network-domain.test.ts
bunx tsc -p tsconfig.json --noEmit
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
5. Assert `k3h4Runtime.mode` and `k3h4Runtime.spectralActivation` remain unchanged across repeated runs.

## 8) Rollback drill
1. Set `BANANA_NETCODE_K3H4_ENABLED=false`.
2. Optionally set `BANANA_NETCODE_K3H4_COHORT=rollback` for evidence tagging.
3. Re-run API contract test for `/api/netcode/analytics`.
4. Verify non-K3H4 payload path is preserved and UI shows unavailable state for the analytics panel.

## 9) Evidence capture checklist

Collect and store evidence in:

- Native: `artifacts/native/k3h4/determinism/<timestamp>/`
- API: `artifacts/api/035-native-k3h4/<timestamp>/`

Recommended capture commands:

```bash
cd /c/Github/Banana
ctest -C Debug --test-dir out/v3-native -R "netcode|k3h4" --output-on-failure | tee artifacts/native/k3h4/determinism/<timestamp>/ctest.log

cd src/typescript/api
bun test src/routes/netcode.contract.test.ts src/routes/netcode.integration.test.ts src/services/nativeNetcode.fail-fast.test.ts | tee ../../../artifacts/api/035-native-k3h4/<timestamp>/api-contract.log

cd ../react
bun test src/domain/notebook/useNetcodeSession.test.ts src/domain/notebook/network-domain.test.ts | tee ../../../artifacts/api/035-native-k3h4/<timestamp>/react-consumer.log
```
