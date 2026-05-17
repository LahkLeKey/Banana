# Research: V3 Runtime Baseline Reset

## Decision 1: Treat the current repository tree as the only active V3 runtime authority

**Decision**: Use the live source tree plus active spec pointers as the baseline authority for V3 planning:

- `.specify/specs/052-v3-pure-c-typescript-reset/spec.md`
- `.specify/specs/README.md`
- `.specify/feature.json`
- `src/native/**`
- `src/typescript/**`
- `.specify/specs/052-v3-pure-c-typescript-reset/codebase-scan.md`

**Rationale**:

- The active spec root names Spec 052 as the sole active baseline.
- `.specify/feature.json` points to Spec 052.
- Current source paths expose only scaffold runtime surfaces, so planning that assumes richer behavior would be speculative rather than repository-grounded.
- `codebase-scan.md` now records the exact retained capability inventory and evidence paths used by downstream planners.

**Alternatives considered**:

- Continue treating Specs 050/051 as active authority: rejected because they are explicitly superseded and archived.
- Use planning notes or heartbeat claims as runtime truth when source disagrees: rejected because this feature exists to eliminate conflicting speculative authority.

## Decision 2: Define the retained runtime baseline as a scaffolded native core plus scaffolded managed shells

**Decision**: The retained V3 baseline is intentionally minimal:

- Native C baseline:
  - `src/native/CMakeLists.txt`
  - `src/native/include/banana_native_v3.h`
  - `src/native/scaffold/native_entry.c`
- Managed shell baseline:
  - `src/typescript/api/src/index.ts`
  - `src/typescript/react/src/index.ts`
  - `src/typescript/electron/main.js`
  - `src/typescript/react-native/index.ts`
  - `src/typescript/shared/ui/src/index.ts`

**Rationale**:

- The native surface currently exports only `banana_native_v3_abi_version()` and `banana_native_v3_ping()`.
- The TypeScript packages expose placeholder entry points and echo-only build/dev scripts.
- This matches `codebase-scan.md`, which describes the reset as a clean-slate scaffold rather than an incremental legacy carry-forward.

**Alternatives considered**:

- Reconstruct a richer runtime baseline from archived legacy code: rejected because archived code is lineage, not active authority.
- Treat current shells as full customer-facing gameplay features: rejected because the code only supports scaffold-level behavior.

## Decision 3: Classify stale viewport-first and legacy-rich pathways as non-authoritative lineage

**Decision**: Any viewport-first, engine-first, or pre-reset capability claim is lineage-only unless it is backed by a current source path or explicitly reintroduced in a later V3 slice.

**Rationale**:

- The spec explicitly excludes stale viewport-first assumptions from baseline correctness.
- `.legacy/` and `.specify/legacy-baseline/` contain historical material, but current source paths no longer implement those systems.
- Existing planning artifacts that mention functions such as `startSession`, `apiHealth`, or `normalizeMovement` are useful context, but they are not runtime authority without matching live source evidence.

**Migration note**:

- Keep those historical names available for archive review only.
- Do not reuse them as retained-capability names until a current source path exposes equivalent behavior.

**Alternatives considered**:

- Keep legacy viewport or engine pathways as default planning assumptions: rejected because that would reintroduce the conflicting baseline this reset removes.
- Delete all lineage artifacts: rejected because traceable historical context is still required by the spec.

## Decision 4: Use four bounded execution lanes with explicit stitch points

**Decision**: Organize downstream planning into:

1. Native gameplay
2. API gameplay
3. Frontend gameplay
4. Shared gameplay contracts

Reserve cross-lane work for explicit stitch slices only.

**Rationale**:

- The current source tree already decomposes into these four active surfaces.
- The spec requires every retained capability to belong to exactly one lane.
- This keeps controller/service/pipeline/native-interop layering stable as actual implementation returns.

**Alternatives considered**:

- Plan all V3 work as one monolithic runtime slice: rejected because it would blur ownership and reintroduce non-gameplay coupling.
- Split by platform only (web/electron/mobile/native): rejected because shared contracts and API orchestration need distinct ownership boundaries.

## Decision 5: Keep validation minimal and evidence-driven until gameplay surfaces are reintroduced

**Decision**: Use the current scaffold validation contract as the planning baseline:

- Spec preflight + confidence gate
- Native CMake configure/build smoke
- Bun/Node import/build smokes for managed shells

**Rationale**:

- These are the only validations supported by the current scaffold reality.
- Adding broader validation assumptions now would invent unimplemented runtime scope.
- The spec needs traceable evidence more than speculative test depth at this stage.

**Alternatives considered**:

- Require full gameplay integration suites now: rejected because the active baseline does not yet implement gameplay flows.
- Skip validation entirely for planning artifacts: rejected because the constitution requires verifiable gates and heartbeat evidence.
