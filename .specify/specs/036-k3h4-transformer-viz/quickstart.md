# Quickstart: K3H4 Transformer-Comparative Visualizations

**Feature**: `036-k3h4-transformer-viz`
**Branch**: `feat/036-k3h4-transformer-viz`

---

## Prerequisites

- Native build toolchain: CMake, MSVC or GCC/Clang
- Bun (TypeScript runtime + package manager)
- Docker Desktop (for compose runtime validation)
- Existing 035 k3h4 native build in `out/v3-native` (or rebuild below)

---

## Step 1 — Build Native (includes new benchmark + export hook)

```bash
# From repo root
cmake -S src/native -B out/v3-native
cmake --build out/v3-native
```

Or use the VS Code task: **Native: build (v3)**

---

## Step 2 — Run Scaling Benchmark (generates artifact for API)

```bash
ctest -C Debug --test-dir out/v3-native -R banana_k3h4_scaling_benchmark --output-on-failure
```

This writes `artifacts/native/k3h4-scaling-benchmark.json`.

---

## Step 3 — Run K3H4 Native Tests (includes export hook tests)

```bash
ctest -C Debug --test-dir out/v3-native -R "netcode|k3h4" --output-on-failure
```

---

## Step 4 — Run API Contract Tests

```bash
cd src/typescript/api
bun test src/routes/netcode.contract.test.ts
bun test src/services/k3h4TrainingService.test.ts
```

---

## Step 5 — Start Local API + React

```bash
# Docker Compose overworld profile (API + React)
docker compose up -d api-overworld react-overworld
```

Or use VS Code task: **Runtime: up (overworld compose)**

---

## Step 6 — Verify Scaling Curve Chart

Open the React shell in browser:
```
http://localhost:5173
```

Navigate to **Notebook → Scaling Curve** to verify the log-log chart renders with both series.

---

## Step 7 — Run a Training Session (manual smoke)

```bash
# Create session
curl -X POST http://localhost:3000/api/netcode/k3h4/training-session \
  -H 'Content-Type: application/json' \
  -d '{"mode":"power"}'

# Check confidence (empty initially)
curl http://localhost:3000/api/netcode/k3h4/training-session/<session-id>/confidence
```

---

## Artifact Paths Reference

| Artifact | Path |
|---|---|
| Scaling benchmark JSON | `artifacts/native/k3h4-scaling-benchmark.json` |
| Training artifact (per epoch) | `artifacts/native/k3h4-training/<session_id>/epoch-<N>.bin` |

---

## Validation Entry Points

| Check | Command |
|---|---|
| Native benchmark + export tests | `ctest --test-dir out/v3-native -R "k3h4" --output-on-failure` |
| API contract tests | `bun test` in `src/typescript/api` |
| React consumer tests | `bun test` in `src/typescript/react` |
| Determinism snapshot | API contract test `k3h4-viz-determinism.contract.test.ts` |
| Slope validation (SC-002) | Native CTest `banana_k3h4_scaling_benchmark` log output |
