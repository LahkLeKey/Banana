# Implementation Plan: Native K3H4 Analytics

**Branch**: `035-native-k3h4` | **Date**: 2026-06-13 | **Spec**: `.specify/specs/035-native-k3h4/spec.md`

**Input**: Feature specification from `.specify/specs/035-native-k3h4/spec.md`

## Summary

Implement deterministic fixed-point K-means compute in Banana native netcode runtime, expose inscribed radius, weighted Voronoi score, and spectral proxy outputs through an endianness-safe versioned ABI envelope, orchestrate production compute through API routes/services only, and keep React as a presentation-only consumer with deterministic rollout/testing gates.

## Technical Context

**Language/Version**: C17 (native runtime/ABI), TypeScript 5.x + Bun runtime (API + React)

**Primary Dependencies**:
- Native: CMake toolchain under `src/native`, runtime netcode modules (`engine/runtime/netcode/*`), ABI bridge (`engine/runtime/abi/netcode/*`)
- API: Fastify routes/services in `src/typescript/api/src/routes` and `src/typescript/api/src/services`
- React: hooks/components under `src/typescript/react/src/domain/notebook`

**Storage**: N/A for primary compute path (in-memory deterministic compute). Existing service persistence remains unchanged.

**Testing**:
- Native: CTest suites in `out/v3-native` with focused `netcode|k3h4` coverage additions
- API: Bun route/service contract tests (e.g., `src/typescript/api/src/routes/netcode.contract.test.ts`)
- React: Bun consumer tests in `src/typescript/react/src/domain/notebook`

**Target Platform**:
- Primary compute runtime: Windows + Linux native builds via CMake
- API orchestration: Bun/Fastify service environments (local + container)
- Consumer: React web shell on supported Banana runtime channels

**Project Type**: Multi-project monorepo feature spanning native library + web API + web client consumer

**Performance Goals**:
- Determinism: 100% byte-identical payloads for identical inputs/configuration
- Service objective alignment: at least 95% of valid requests within existing analytics latency objectives (SC-004)

**Constraints**:
- Production clustering math must execute only in native/API orchestration path
- ABI payloads must remain endianness-safe and versioned
- Output ordering must be deterministic across clusters/metrics
- Failure states must be explicit and non-partial

**Scale/Scope**:
- Feature scope: netcode analytics route/service plus native netcode vector/k3h4/ABI path
- Expected cardinality: bounded vector dimensions (`<=16`) and deterministic cluster counts for runtime analytics requests
- Touched domains: `src/native`, `src/typescript/api`, `src/typescript/react`

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

### Pre-Phase 0 Gate

- [x] Player trust and disclosure alignment verified.
  - Outcome: Internal analytics behavior change only; no new storefront claims or disclosure copy required.
- [x] Storefront governance artifacts identified when public Steam copy is affected.
  - Outcome: Not applicable; this feature does not alter public Steam copy.
- [x] Cross-domain contracts mapped for touched layers.
  - Outcome: Mapped native runtime compute -> ABI envelope -> API route/service response -> React presentation contract.
- [x] Quality gates defined with measurable checks.
  - Outcome: Determinism, endianness conformance, invalid payload handling, and presentation-only consumer checks are scoped.
- [x] Reproducible delivery path identified.
  - Outcome: CMake + Bun + existing compose/runtime paths documented in quickstart.

### Post-Phase 1 Re-check

- [x] Research/design artifacts explicitly preserve controller -> service -> pipeline -> native interop flow.
- [x] Versioned ABI + API contracts capture deterministic errors and ordering guarantees.
- [x] Test strategy covers native, API orchestration, and React presentation-only requirements.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/035-native-k3h4/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   ├── native-k3h4-abi.md
│   └── api-netcode-k3h4.md
└── tasks.md
```

### Source Code (repository root)

```text
src/native/
├── include/banana_native_v3.h
├── engine/runtime/netcode/vector/
├── engine/runtime/netcode/k3h4/
├── engine/runtime/abi/netcode/
└── scaffold/native_entry.c

src/typescript/api/src/
├── routes/netcode.ts
├── routes/netcode.contract.test.ts
└── services/nativeNetcode.ts

src/typescript/react/src/
└── domain/notebook/
   ├── useNetcodeSession.ts
   ├── network-domain.ts
   └── (consumer tests)

tests/
└── native/
   └── (determinism + ABI/endianness fixtures to be added)
```

**Structure Decision**: Use Banana monorepo multi-domain structure and keep compute ownership centralized in native/API while React remains a read/render consumer. No new top-level projects are introduced.

## Phase Breakdown

### Phase 0: Research and Clarification (Completed)

Output: `.specify/specs/035-native-k3h4/research.md`

- Finalized fixed-point representation and deterministic tie-break/order policies.
- Defined inscribed radius guardrails for single-cluster and near-zero radius edge cases.
- Chosen versioned little-endian ABI envelope strategy with explicit decode errors.
- Confirmed API-only production compute and React presentation-only consumption policy.

### Phase 1: Design and Contracts (Completed)

Outputs:
- `.specify/specs/035-native-k3h4/data-model.md`
- `.specify/specs/035-native-k3h4/contracts/native-k3h4-abi.md`
- `.specify/specs/035-native-k3h4/contracts/api-netcode-k3h4.md`
- `.specify/specs/035-native-k3h4/quickstart.md`
- `.specify/specs/035-native-k3h4/reorchestration-next-steps.md`

Design outcomes:
- Data entities and state transitions defined for batch, centers, radii, scores, spectral proxies, and observability.
- Native ABI envelope layout/version/error semantics specified for endianness-safe transport.
- API request/response/error contract defined with rollout-aware behavior.
- Validation entry points documented for native/API/React deterministic checks.
- Follow-up reorchestration direction documented for dual assignment modes, spectral gating, and hardware preflight ordering.

### Phase 2: Implementation Planning (Ready for `/speckit.tasks`)

1. Native workstream
   - Add fixed-point K-means loop and deterministic assignment ordering in netcode runtime.
   - Extend K3H4 metric modules for inscribed radius, weighted Voronoi score, spectral proxy outputs.
   - Add ABI envelope encode/decode helpers with endianness/version/CRC validation.
2. API workstream
   - Extend `nativeNetcode` service types and binary decode mapping for new envelope fields.
   - Update `netcode` route orchestration and rollout flags for staged activation/rollback.
   - Add/expand contract tests for malformed payloads, unsupported versions, and determinism snapshots.
3. React workstream
   - Update netcode session hook and visualization components to render orchestrated K-means metrics.
   - Add tests proving no production local recompute fallback.
4. Validation/evidence workstream
   - Add deterministic native test fixtures and mixed-endianness conformance fixtures.
   - Capture rollout evidence for staged enablement and rollback drill.

## Storefront Deliverables

Not required for this feature. No public Steam/storefront copy or asset changes are in scope.

## Complexity Tracking

No constitution violations or justified exceptions recorded.
