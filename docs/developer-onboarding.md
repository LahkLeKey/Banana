# Banana Developer Onboarding

## Repository Domains

Banana is a multi-domain monorepo:

- Native runtime and ABI: `src/native`
- TypeScript API orchestration: `src/typescript/api`
- React presentation shell: `src/typescript/react`
- Delivery/runtime automation: `docker`, `scripts`, `.github/workflows`

## Runtime Contract Flow

Preserve this flow for gameplay and analytics contracts:

1. Route layer validates request shape and rollout gates.
2. Service layer orchestrates domain computation and error mapping.
3. Pipeline/native interop layer performs deterministic compute through FFI.
4. Native ABI envelope returns versioned, endianness-safe payloads.
5. API maps native output to stable JSON contracts.
6. React consumes API output as presentation-only state.

## Deterministic Netcode Analytics (Feature 035)

The `/api/netcode/analytics` route is authoritative for hypersphere K-means metrics.

- Contract fields include `contractVersion`, `lspRepresentation`, `reward`, `link`, `vector`, `hypersphere`, `hypersphereKmeans`, and `rollout`.
- `hypersphereKmeans` includes centers, radii, weighted Voronoi scores, spectral proxy, and observability fields.
- ABI reliability failures are mapped to deterministic API errors:
  - `ERR_UNSUPPORTED_VERSION`
  - `ERR_BAD_CRC`
  - `ERR_PAYLOAD_TRUNCATED`
- Rollout disabled behavior returns HTTP `503` with rollout metadata and should render an unavailable analytics state in React.

## Required Environment Contracts

- `BANANA_NATIVE_PATH`: explicit native library location for API FFI.
- `BANANA_NETCODE_ADAPTER=ffi`: enables native FFI path.
- `BANANA_NETCODE_HYPERSPHERE_KMEANS_ENABLED=true|false`: rollout gate.
- `BANANA_NETCODE_HYPERSPHERE_KMEANS_COHORT=<cohort>`: staged rollout marker.
- `VITE_BANANA_API_BASE_URL`: React API base URL contract.

## Validation Entry Points

Native:

```bash
cmake -S src/native -B out/v3-native
cmake --build out/v3-native
ctest -C Debug --test-dir out/v3-native -R "netcode|hypersphere|kmeans" --output-on-failure
```

API:

```bash
cd src/typescript/api
bun test src/routes/netcode.contract.test.ts src/routes/netcode.integration.test.ts src/services/nativeNetcode.fail-fast.test.ts
```

React:

```bash
cd src/typescript/react
bun test src/domain/notebook/useNetcodeSession.test.ts src/domain/notebook/network-domain.test.ts
bunx tsc -p tsconfig.json --noEmit
```
