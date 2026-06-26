# K3H4 One-Shot Move Manifest

This is the approved source-to-destination move plan for the K3H4 core extraction into `modules/k3h4`.

## Scope lock

- Core-only move: yes
- New root: `modules/k3h4`
- Migration style: one-shot
- Include style after move: module-prefixed public includes
- Public target: `banana_k3h4_core`
- Tests move with module: yes

## File moves (source -> destination)

### Native K3H4 core

- `src/native/k3h4/k3h4_native_orchestrator.c` -> `modules/k3h4/native/src/k3h4/k3h4_native_orchestrator.c`
- `src/native/k3h4/k3h4_native_orchestrator.h` -> `modules/k3h4/native/src/k3h4/k3h4_native_orchestrator.h`
- `src/native/k3h4/k3h4_metrics_orchestration_layer.c` -> `modules/k3h4/native/src/k3h4/k3h4_metrics_orchestration_layer.c`
- `src/native/k3h4/k3h4_metrics_orchestration_layer.h` -> `modules/k3h4/native/src/k3h4/k3h4_metrics_orchestration_layer.h`
- `src/native/k3h4/application/k3h4_metrics_application_service.c` -> `modules/k3h4/native/src/k3h4/application/k3h4_metrics_application_service.c`
- `src/native/k3h4/application/k3h4_metrics_application_service.h` -> `modules/k3h4/native/src/k3h4/application/k3h4_metrics_application_service.h`
- `src/native/k3h4/domain/k3h4_metrics_domain_port.h` -> `modules/k3h4/native/src/k3h4/domain/k3h4_metrics_domain_port.h`
- `src/native/k3h4/infrastructure/k3h4_metrics_infrastructure_adapter.c` -> `modules/k3h4/native/src/k3h4/infrastructure/k3h4_metrics_infrastructure_adapter.c`
- `src/native/k3h4/infrastructure/k3h4_metrics_infrastructure_adapter.h` -> `modules/k3h4/native/src/k3h4/infrastructure/k3h4_metrics_infrastructure_adapter.h`
- `src/native/k3h4/README.md` -> `modules/k3h4/native/README.md`

### Native runtime netcode K3H4

- `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_cluster_scoring.c` -> `modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_cluster_scoring.c`
- `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_cluster_scoring.h` -> `modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_cluster_scoring.h`
- `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_export.c` -> `modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_export.c`
- `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_export.h` -> `modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_export.h`
- `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_geometry.c` -> `modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_geometry.c`
- `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_geometry.h` -> `modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_geometry.h`
- `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_metrics.c` -> `modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_metrics.c`
- `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_metrics.h` -> `modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_metrics.h`
- `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_observability.c` -> `modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_observability.c`
- `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_observability.h` -> `modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_observability.h`
- `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_orchestrator.c` -> `modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_orchestrator.c`
- `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_orchestrator.h` -> `modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_orchestrator.h`
- `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_pipeline.c` -> `modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_pipeline.c`
- `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_pipeline.h` -> `modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_pipeline.h`
- `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_pipeline_internal.h` -> `modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_pipeline_internal.h`
- `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_pipeline_setup.c` -> `modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_pipeline_setup.c`
- `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_pipeline_setup.h` -> `modules/k3h4/native/src/runtime/netcode/k3h4/netcode_k3h4_pipeline_setup.h`

### Native runtime vector bridge

- `src/native/engine/runtime/netcode/vector/netcode_k3h4.c` -> `modules/k3h4/native/src/runtime/netcode/vector/netcode_k3h4.c`
- `src/native/engine/runtime/netcode/vector/netcode_k3h4.h` -> `modules/k3h4/native/src/runtime/netcode/vector/netcode_k3h4.h`

### K3H4 native tests

- `tests/native/k3h4/k3h4_metrics_orchestration_layer_test.c` -> `modules/k3h4/native/tests/k3h4/k3h4_metrics_orchestration_layer_test.c`
- `tests/native/k3h4/infrastructure/k3h4_metrics_infrastructure_adapter_test.c` -> `modules/k3h4/native/tests/k3h4/infrastructure/k3h4_metrics_infrastructure_adapter_test.c`
- `tests/native/runtime/ai/combat/combat_controller_k3h4_test.c` -> `modules/k3h4/native/tests/runtime/ai/combat/combat_controller_k3h4_test.c`
- `tests/native/runtime/netcode/netcode_k3h4_assignment_family_test.c` -> `modules/k3h4/native/tests/runtime/netcode/netcode_k3h4_assignment_family_test.c`
- `tests/native/runtime/netcode/netcode_k3h4_covariance_ellipsoid_test.c` -> `modules/k3h4/native/tests/runtime/netcode/netcode_k3h4_covariance_ellipsoid_test.c`
- `tests/native/runtime/netcode/netcode_k3h4_determinism_test.c` -> `modules/k3h4/native/tests/runtime/netcode/netcode_k3h4_determinism_test.c`
- `tests/native/runtime/netcode/netcode_k3h4_edge_cases_test.c` -> `modules/k3h4/native/tests/runtime/netcode/netcode_k3h4_edge_cases_test.c`
- `tests/native/runtime/netcode/netcode_k3h4_export_io_failure_test.c` -> `modules/k3h4/native/tests/runtime/netcode/netcode_k3h4_export_io_failure_test.c`
- `tests/native/runtime/netcode/netcode_k3h4_export_test.c` -> `modules/k3h4/native/tests/runtime/netcode/netcode_k3h4_export_test.c`
- `tests/native/runtime/netcode/netcode_k3h4_orchestrator_failure_paths_test.c` -> `modules/k3h4/native/tests/runtime/netcode/netcode_k3h4_orchestrator_failure_paths_test.c`
- `tests/native/runtime/netcode/netcode_k3h4_orchestrator_test.c` -> `modules/k3h4/native/tests/runtime/netcode/netcode_k3h4_orchestrator_test.c`
- `tests/native/runtime/netcode/netcode_k3h4_pipeline_equivalence_test.c` -> `modules/k3h4/native/tests/runtime/netcode/netcode_k3h4_pipeline_equivalence_test.c`
- `tests/native/runtime/netcode/netcode_k3h4_pipeline_setup_test.c` -> `modules/k3h4/native/tests/runtime/netcode/netcode_k3h4_pipeline_setup_test.c`
- `tests/native/runtime/netcode/netcode_k3h4_scaling_benchmark_test.c` -> `modules/k3h4/native/tests/runtime/netcode/netcode_k3h4_scaling_benchmark_test.c`

## Rewire-only files (no move, update references)

- `src/native/CMakeLists.txt`
- `src/native/engine/CMakeLists.txt`
- `tests/native/CMakeLists.txt`
- `scripts/build-k3h4-standalone-release.sh`
- `.github/workflows/k3h4-model-release.yml`
- TypeScript adapters/routes that call K3H4 native APIs under:
  - `src/typescript/api/src/**`
  - `src/typescript/react/src/**`

## Public include policy after move

- External consumers must include module-prefixed public headers only (for example under `banana/k3h4/...`).
- External consumers must link only against `banana_k3h4_core`.
- Direct includes into module internals are not allowed.
