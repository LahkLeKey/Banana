# K3H4 One-Shot Migration Execution Plan

This plan executes the approved one-shot migration from legacy native paths into `modules/k3h4`.

Decision baseline:

- Core-only split
- New root at `modules/k3h4`
- One-shot migration
- Tests move with module
- Public target: `banana_k3h4_core`
- Public include style: module-prefixed only
- Merge gate excludes war suite

Reference manifest: `modules/k3h4/MOVE-MANIFEST.md`

## Success criteria

1. All files listed in the move manifest are moved.
2. Native build succeeds.
3. K3H4 native test targets pass.
4. API smoke passes.
5. UI smoke passes.
6. No external consumer includes module internals directly.

## Phase 0: Pre-flight

Run from repo root:

1. `npm run kickoff:status`
2. If not ready: `npm run kickoff:return:auto-fix`
3. Confirm branch is clean enough for large move:
   - `git status --short`

## Phase 1: Create module build skeleton

Create files:

1. `modules/k3h4/CMakeLists.txt`
2. `modules/k3h4/native/CMakeLists.txt`
3. `modules/k3h4/native/tests/CMakeLists.txt`

Required behavior:

- `modules/k3h4/native/CMakeLists.txt` defines static library `banana_k3h4_core`.
- `banana_k3h4_core` compiles all moved K3H4 native/runtime source files from the manifest.
- Public includes are exposed from `modules/k3h4/native/include` using module-prefixed paths.
- `modules/k3h4/native/tests/CMakeLists.txt` defines moved K3H4 tests and links to `banana_k3h4_core` plus existing required engine/native targets.

## Phase 2: Move files exactly as defined

Execute the source-to-destination moves from:

- `modules/k3h4/MOVE-MANIFEST.md`

Move groups in this order:

1. Native K3H4 core files
2. Runtime netcode K3H4 files
3. Vector bridge files
4. K3H4 test files

After each group:

- `git status --short`
- verify destination file count matches source list for that group

## Phase 3: Rewire native build targets

Edit `src/native/CMakeLists.txt`:

1. Remove direct `k3h4/...` source entries from `banana_native`.
2. Link `banana_native` against `banana_k3h4_core`.
3. Add module include root if needed for public headers only.
4. Ensure install/export behavior is unchanged for existing native ABI headers.

Edit `src/native/engine/CMakeLists.txt`:

1. Remove direct `runtime/netcode/k3h4/...` and `runtime/netcode/vector/netcode_k3h4.*` entries from engine source lists.
2. Link `banana_engine_core` against `banana_k3h4_core`.
3. Update `banana_native_feedback_loop_factory` to stop compiling legacy `../k3h4/...` sources directly and rely on linked module target.
4. Keep existing BLAS option and behavior semantics unchanged.

## Phase 4: Rewire native tests

Edit `tests/native/CMakeLists.txt`:

1. Update source paths for moved K3H4 tests to `modules/k3h4/native/tests/...`.
2. For tests that previously compiled K3H4 implementation files directly, switch to linking `banana_k3h4_core` unless test explicitly requires source-level override.
3. Keep existing non-K3H4 tests untouched.
4. Ensure test names remain stable where possible to avoid CI/reporting churn.

## Phase 5: Rewire release script/workflow paths

Edit `scripts/build-k3h4-standalone-release.sh`:

1. Keep output artifact paths unchanged under `artifacts/native/k3h4/releases/...`.
2. Update any source/header assumptions if module move changes lookup paths.
3. Preserve CLI contract and output variable names.

Edit `.github/workflows/k3h4-model-release.yml`:

1. Keep workflow name and dispatch inputs unchanged.
2. Update build assumptions only as needed for new module wiring.
3. Preserve artifact and release naming semantics.

## Phase 6: Include policy enforcement

Apply public-boundary policy:

1. External consumers may include only module-prefixed public headers.
2. External consumers must link `banana_k3h4_core`.
3. Remove or replace any direct includes into moved internal source trees.

Scope for rewire-only consumers:

- `src/typescript/api/src/**`
- `src/typescript/react/src/**`
- any native/runtime files outside `modules/k3h4` that reference moved headers

## Phase 7: Validation (required merge gate)

Run in this exact order:

1. Native configure:
   - `cmake -S src/native -B out/v3-native`
2. Native build:
   - `cmake --build out/v3-native`
3. K3H4-focused native tests (regex):
   - `ctest -C Debug --test-dir out/v3-native -R "k3h4" --output-on-failure`
4. API smoke:
   - `npm run kickoff:return`

Pass condition:

- Native build passes
- K3H4 test regex pass set is green
- API/UI smoke path is green

## Phase 8: Final checks and PR hygiene

1. `git status --short` should include only intentional migration changes.
2. Verify no stale path references remain:
   - search for `src/native/k3h4/`
   - search for `runtime/netcode/k3h4/` legacy roots outside module
3. Confirm docs still align:
   - `CONTEXT-MAP.md`
   - `modules/k3h4/CONTEXT.md`
   - `docs/adr/0001-k3h4-core-module-boundary.md`
4. Prepare one PR with clear sections:
   - file moves
   - build rewiring
   - test rewiring
   - validation evidence

## Rollback strategy

If validation fails at any phase:

1. Stop adding new edits.
2. Fix within current phase scope only.
3. Re-run phase-local validation before advancing.
4. If failure cause is unclear, revert only the most recent phase edits (not the full branch) and retry.
