# Native Module Split Epic (Program Plan)

## Objective

Split the remaining native monolith into domain-first modules using the modules/k3h4 boundary style while preserving current ABI behavior and runtime outcomes.

## Tracker

- Epic issue: #1205
- Story issues: #1189, #1190, #1191, #1192, #1193, #1194, #1195, #1196, #1197, #1198, #1199, #1200, #1201, #1202, #1203, #1204

## Current Progress

- M0 guardrails: complete (strict boundary gate active in CI/local).
- #1192 native_math_foundation: complete.
	- Q16 fixed-point implementation moved to `modules/native_math_foundation/native`.
	- `banana_native_math_foundation` linked into `banana_k3h4_core`.
	- strict boundary validation and native target builds pass.
- #1193 native_runtime_kernel: complete.
	- parallel contract/model/interlock implementations moved to `modules/native_runtime_kernel/native`.
	- `banana_native_runtime_kernel` linked into `banana_engine_core`.
	- tests rewired to link the module target instead of compiling moved sources directly.
	- strict boundary validation, native build, and focused runtime/k3h4 tests pass.
- #1194 foundation rewiring: complete.
	- consumers link foundation module targets (`banana_native_math_foundation`, `banana_native_runtime_kernel`) instead of compiling moved internals directly.
	- strict boundary gate and focused regression suite remain green after rewiring.
- #1195 k3h4 DAG stabilization: complete.
	- no active direct consumption of `modules/k3h4/native/src` in primary native consumer CMake wiring.
	- k3h4 targeted test regex passes (22/22).
	- strict boundary validation remains clean.
- #1196 native_physics_core: complete.
	- created `banana_native_physics_core` module target and wired it into native configure/build.
	- moved physics implementations (`body`, `collider_*`, `dynamics`, `raycast`, `world`, `world_collision`) into module ownership.
	- rewired `banana_engine_core`, `banana_physics_collider_narrow_phase_test`, and `banana_runtime_physics_world_collision_test` to consume physics via module target linking.
	- replaced the world-collision direct-source test seam with a resolver hook so tests no longer compile engine-owned internals directly.
	- validation passes: native build, strict boundary gate, and targeted `physics|k3h4` test regex (28/28).
- #1197 native_world_core: complete.
	- created `banana_native_world_core` module target and wired it into native configure/build.
	- moved world-domain implementations (`world`, `entity`, `world_query`, `signals`) into module ownership.
	- rewired `banana_engine_core` and `banana_world_signals_test` to consume world logic through module target linking.
	- validation passes: native build, strict boundary gate, and targeted `world|signals|physics|k3h4` test regex (33/33).
- #1198 native_ai_core: complete.
	- created `banana_native_ai_core` module target and wired it into native configure/build.
	- moved AI-domain implementations (`controller`, `controller_system_dispatch`, `controller_system_lifecycle`, `combat_controller`, `navigation`, `npc_merchant`, `perception`, `state_machine`, `wildlife_controller`) into module ownership.
	- rewired AI-focused tests to link `banana_native_ai_core` instead of compiling moved AI internals directly.
	- replaced allocator-failure direct-source seam with a controller-system calloc hook API to keep strict boundary policy clean.
	- validation passes: native build, strict boundary gate, and targeted `ai|controller|wildlife|combat|npc|perception|state_machine` test regex (30/30).
- #1199 native_netcode_runtime: complete.
	- created `banana_native_netcode_runtime` module target and wired it into native configure/build.
	- moved netcode runtime implementations (`netcode_model`, `netcode_reward`, `netcode_link`, `netcode_vector`, `netcode_contract`, `netcode_abi`) into module ownership.
	- rewired `banana_engine_core` and `banana_runtime_netcode_vector_failure_path_test` to consume netcode logic through module target linking.
	- replaced the vector failure-path direct-source seam with a k3h4 compute hook API so tests no longer compile netcode internals directly.
	- validation passes: native build, strict boundary gate, explicit module/seam target build, and targeted `netcode|k3h4` test regex (27/27).
	- workflow drift corrected: stale continuity-suite task/doc references were updated to the current supported `netcode|k3h4` suite because no matching continuity tests exist in the present repo/build catalog.
- #1200 native_render_runtime: complete.
	- created `banana_native_render_runtime` module target and wired it into native configure/build.
	- moved render orchestration implementations (`render_material`, `render_submit`, `render_tick_orchestration`, `application_service_render_actor_resolver`, `application_service_render_model_cache`, `application_service_render_port`) into module ownership.
	- rewired `banana_engine_core` and render-focused tests to consume render runtime logic through module target linking.
	- validation passes: native build, strict boundary gate, explicit module/seam target build, and targeted `render|renderer|dx12|material` test regex (13/13).
- #1201 native_ui_runtime: complete.
	- created `banana_native_ui_runtime` module target and wired it into native configure/build.
	- moved UI runtime implementations (`ui`, `ui_inventory`, `ui_merchant`, `ui_abi`) into module ownership.
	- rewired UI-focused tests to link `banana_native_ui_runtime` instead of compiling moved UI internals directly.
	- replaced allocator-failure direct-source seam with UI allocator hooks so tests remain boundary-clean.
	- validation passes: native build, strict boundary gate, explicit module/seam target build, UI smoke, and targeted `ui|merchant|inventory|engine_exports_ui_economy` test regex (13/13).
- #1202 ABI wrapper consolidation: complete.
	- kept `banana_native` as the public ABI composition boundary with wrapper-only source ownership (`scaffold/native_entry.c`).
	- linked extracted module targets at the wrapper boundary (`banana_native_runtime_kernel`, `banana_native_physics_core`, `banana_native_world_core`, `banana_native_ai_core`, `banana_native_netcode_runtime`, `banana_native_render_runtime`, `banana_native_ui_runtime`) instead of aggregating module internals as sources.
	- documented wrapper ownership and consumer rules in native ABI context docs.
	- validation passes: native build (`FULL_BUILD_OK_1202`), strict boundary gate, API smoke, UI smoke, and focused ABI/wrapper regex (`abi|engine_exports|native_feedback_loop_factory`, 19/19).
- #1203 ABI contract freeze: complete.
	- codified public ABI freeze handling in native CMake via explicit `BANANA_NATIVE_PUBLIC_ABI_HEADERS` and stable `BANANA_NATIVE_PUBLIC_ABI_INSTALL_DIR` wiring.
	- documented explicit ABI change/versioning process in native ABI context docs, including required review + version bump flow.
	- preserved installed public ABI headers (`banana_native_v3.h`, `banana_native_ui_abi.h`) and install destination contract.
	- validation passes: native build (`FULL_BUILD_OK_1203`), strict boundary gate, API smoke, UI smoke, and focused ABI/contract regex (`abi|contract|native_feedback_loop_factory`, 18/18).
- #1204 program closeout strict enforcement: complete.
	- confirmed boundary baseline file is removed (no migration exceptions active).
	- verified CI-equivalent strict gate invocation (`bash scripts/validate-native-module-boundaries.sh`) passes with zero violations.
	- published final boundary closeout report confirming no cross-module internal include usage.

## Locked Decisions

- Module design style: domain-first modules; technical layers remain internal.
- Dependency rule: strict acyclic module DAG.
- Consumption model: target-based link plus module-prefixed public headers only.
- Migration strategy: phased one-shot extraction per module.
- Ordering policy: dependency pressure and volatility over current folder names.
- Per-phase governance: build + boundary scan + targeted tests + API/UI smoke.

## Scope

In scope:

- Native runtime module extraction under src/native/engine into explicit module roots.
- Build/test rewiring so consumers link module targets rather than compiling internals.
- Boundary compliance enforcement for includes and link usage.

Out of scope:

- API route/domain ownership changes.
- React feature ownership changes.
- New gameplay features unrelated to extraction.

## Proposed Module Taxonomy

Foundation modules:

1. native_math_foundation
2. native_runtime_kernel

Domain modules:

1. native_k3h4_core (already established)
2. native_physics_core
3. native_world_core
4. native_ai_core

Orchestration modules:

1. native_netcode_runtime
2. native_render_runtime
3. native_ui_runtime

Boundary wrapper:

1. banana_native (shared ABI wrapper target)

## Milestones

## M0 - Program Guardrails

Stories:

1. Define canonical module list, ownership map, and DAG file.
2. Add include-boundary scanner for cross-module internal include detection.
3. Add CI/local gate command that fails on boundary violations.

Acceptance:

1. Module DAG is versioned and reviewed.
2. Boundary scanner fails on a seeded violation fixture.
3. Existing tree passes scanner before module moves begin.

## M1 - Foundation Extraction

Stories:

1. Extract math primitives into native_math_foundation.
2. Extract reusable runtime kernel primitives into native_runtime_kernel.
3. Rewire downstream modules to link the two new targets.

Acceptance:

1. No module depends upward on domain/orchestration modules.
2. Native configure/build passes.
3. Foundation-focused test regex passes.

## M2 - Domain Core Extractions

Stories:

1. Complete k3h4 stabilization against the new DAG expectations.
2. Extract physics core domain logic into native_physics_core.
3. Extract world-state domain logic into native_world_core.
4. Extract AI domain logic into native_ai_core.

Acceptance:

1. External consumers no longer include moved internal paths.
2. Domain module test regexes pass for each extracted module.
3. Build remains green at the end of each module phase.

## M3 - Orchestration Runtime Extractions

Stories:

1. Extract netcode orchestration into native_netcode_runtime.
2. Extract render orchestration into native_render_runtime.
3. Extract UI orchestration into native_ui_runtime.

Acceptance:

1. Orchestration modules only depend on allowed DAG predecessors.
2. Transition and continuity test suites remain green.
3. API/UI smoke checks stay green across all phases.

## M4 - ABI Wrapper Consolidation

Stories:

1. Ensure banana_native is only a composition/wrapper boundary.
2. Remove any remaining direct source aggregation from wrapper target.
3. Freeze module public include contracts and document consumer rules.

Acceptance:

1. Wrapper target links modules; it does not own extracted internals.
2. Public ABI headers remain installed and unchanged unless explicitly versioned.
3. Contract smoke path is green.

## Phase Gate (Definition of Done per extraction phase)

1. cmake -S src/native -B out/v3-native succeeds.
2. cmake --build out/v3-native succeeds.
3. Boundary compliance scan reports zero violations.
4. Targeted native test regex for touched module passes.
5. API smoke and UI smoke pass.

## Program Risks and Controls

Risk: hidden dependency cycles emerge during rewiring.
Control: fail-fast DAG checker in CI and local pre-flight.

Risk: stale internal include paths linger in non-native consumers.
Control: boundary scanner includes native and non-native roots that include native headers.

Risk: test signal is too broad and slows phase velocity.
Control: require targeted regex for merge gate and keep larger suites as nightly confidence.

Risk: extraction drift from vocabulary and ownership intent.
Control: update context glossary terms when boundaries change and record irreversible trade-offs in ADRs.

## Rollback Policy

1. Roll back only the current phase if gate fails.
2. Preserve prior passed phases unchanged.
3. Re-run full phase gate before retrying.

## Epic Backlog Seed (Issue Checklist)

1. Program guardrails: DAG file + scanner + CI gate (tracked by #1189, #1190, #1191).
2. Foundation extraction: math module (tracked by #1192).
3. Foundation extraction: runtime kernel module (tracked by #1193).
4. Foundation rewiring: consumers link foundation modules (tracked by #1194).
5. Domain extraction: k3h4 stabilization (tracked by #1195).
6. Domain extraction: physics core (tracked by #1196).
7. Domain extraction: world core (tracked by #1197).
8. Domain extraction: AI core (tracked by #1198).
9. Orchestration extraction: netcode runtime (tracked by #1199).
10. Orchestration extraction: render runtime (tracked by #1200).
11. Orchestration extraction: UI runtime (tracked by #1201).
12. ABI wrapper consolidation (tracked by #1202).
13. ABI contract freeze (tracked by #1203).
14. Program closeout strict enforcement (tracked by #1204).
