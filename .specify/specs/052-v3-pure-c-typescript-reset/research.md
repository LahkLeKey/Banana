# Research: V3 Runtime Baseline Reset

## Decision 1: Treat the current repository tree as the only active V3 runtime authority

**Decision**: Use the live source tree plus active spec pointers as the baseline authority for V3 planning:

- `.specify/specs/052-v3-pure-c-typescript-reset/spec.md`
- `.specify/specs/052-v3-pure-c-typescript-reset/plan.md`
- `.specify/specs/052-v3-pure-c-typescript-reset/data-model.md`
- `.specify/specs/052-v3-pure-c-typescript-reset/contracts/*.md`
- `.specify/specs/052-v3-pure-c-typescript-reset/codebase-scan.md`
- `.specify/specs/README.md`
- `.specify/feature.json`
- `src/native/**`
- `src/typescript/**`

**Rationale**:

- `.specify/specs/README.md` lists Spec 052 as the sole active baseline.
- `.specify/feature.json` points to Spec 052.
- Current source paths expose scaffold runtime surfaces only, so planning that assumes richer behavior would be speculative rather than repository-grounded.
- The active planning artifacts now describe the same retained capability inventory and lane boundaries as the live source tree.

**Alternatives considered**:

- Continue treating Specs 050/051 as active authority: rejected because they are explicitly superseded and archived.
- Use heartbeat or planning claims as runtime truth when source disagrees: rejected because this feature exists to eliminate conflicting speculative authority.

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
- The TypeScript packages expose placeholder entry points or minimal shell boot functions only.
- This matches the reset intent: planning must describe what is live now, not what used to exist in lineage.

**Alternatives considered**:

- Reconstruct a richer runtime baseline from archived legacy code: rejected because archived code is lineage, not active authority.
- Treat current shells as full customer-facing gameplay features: rejected because the code only supports scaffold-level behavior.

## Decision 3: Classify archived and speculative pathways as lineage-only unless live source evidence reintroduces them

**Decision**: Any archived, stale, or speculative capability claim is lineage-only unless it is backed by a current source path or explicitly reintroduced in a later V3 slice.

**Rationale**:

- The spec explicitly requires authoritative inputs to be distinguished from stale or speculative lineage.
- `.legacy/` and `.specify/legacy-baseline/` preserve historical material, but current source paths no longer implement those systems.
- Historical capability names are useful context, but they are not runtime authority without matching live source evidence.

**Migration note**:

- Keep historical names and archive paths available for comparison only.
- Do not reuse archived capability claims as retained-capability evidence until a live source path exposes equivalent behavior.

**Alternatives considered**:

- Keep archived pathways as default planning assumptions: rejected because that would reintroduce the conflicting baseline this reset removes.
- Delete all lineage artifacts: rejected because traceable historical context is still required by the spec.

## Decision 4: Use four bounded execution lanes with stitch slices only when active capabilities truly cross domains

**Decision**: Organize downstream planning into four owned lanes:

1. Native gameplay
2. API gameplay
3. Frontend gameplay
4. Shared gameplay contracts

Reserve cross-lane work for explicit stitch slices only.

**Rationale**:

- The current source tree already decomposes into these four active surfaces.
- The spec requires every retained capability to belong to exactly one lane.
- This keeps controller -> service -> pipeline -> native interop layering stable as actual implementation returns.

**Alternatives considered**:

- Plan all V3 work as one monolithic runtime slice: rejected because it would blur ownership and reintroduce non-gameplay coupling.
- Split purely by platform labels without a shared contract lane: rejected because shared gameplay contracts need distinct ownership.

## Decision 5: Use the current scaffold validation contract as the planning baseline

**Decision**: Use the smallest existing validations that match live scaffold reality:

- Spec preflight + confidence gate
- Native CMake configure/build smoke
- Package-script build smokes for managed shells

**Rationale**:

- These are the only validations supported by the current scaffold surfaces.
- Adding broader gameplay validation assumptions now would invent unimplemented runtime scope.
- The constitution requires evidence-backed planning, not speculative test depth.

**Alternatives considered**:

- Require full gameplay integration suites now: rejected because the active baseline does not yet implement gameplay flows.
- Skip validation entirely for planning artifacts: rejected because the constitution requires verifiable gates and heartbeat evidence.
