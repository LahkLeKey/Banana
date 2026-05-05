# Tasks: WASM SIMD Advanced Calculation Kernels Spike (258)

**Input**: Spec at `.specify/specs/258-wasm-simd-advanced-calculation-kernels/spec.md`

## Phase 1: Discovery

- [ ] T001 Inventory advanced calculation hotspots and estimate SIMD suitability per hotspot. [FR-001]
- [ ] T002 Define representative workload shapes for kernel benchmarking (small/medium/large). [FR-004]

## Phase 2: Design

- [ ] T003 Define kernel catalog with SIMD/scalar pair strategy and ownership map. [FR-001] [FR-002]
- [ ] T004 Define capability detection contract and deterministic fallback behavior for unsupported runtimes. [FR-002]
- [ ] T005 Define numeric parity policy (absolute/relative error bounds) for SIMD vs scalar validation. [FR-003]

## Phase 3: Validation Planning

- [ ] T006 Define microbenchmark harness and reporting template for per-kernel latency/throughput. [FR-004]
- [ ] T007 Define scenario benchmark suite and release-gate thresholds per kernel family. [FR-004] [FR-005]
- [ ] T008 Define React-first promotion gate that must pass before desktop/mobile SIMD rollout per kernel family. [FR-006]

## Phase 4: Spec Validation

- [ ] T009 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/258-wasm-simd-advanced-calculation-kernels` and confirm `OK`. [FR-001]
- [ ] T010 Run `bash .specify/scripts/bash/validate-spec-boundaries.sh --spec .specify/specs/258-wasm-simd-advanced-calculation-kernels/spec.md` and confirm pass. [FR-002]
- [ ] T011 Run `bash .specify/scripts/bash/validate-task-traceability.sh --spec .specify/specs/258-wasm-simd-advanced-calculation-kernels/spec.md --tasks .specify/specs/258-wasm-simd-advanced-calculation-kernels/tasks.md` and confirm pass. [FR-003]
