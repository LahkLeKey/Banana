# Implementation Plan: Demo Frame QA Evidence

**Branch**: `033-unify-coherent-world` | **Date**: 2026-06-03 | **Spec**: .specify/specs/032-demo-frame-qa/spec.md

**Input**: Feature specification from .specify/specs/032-demo-frame-qa/spec.md

## Summary

Export reviewable demo frames from native demo test runs by extending the DX12 POC and feedback-loop harness to emit frame bundles (image files plus metadata manifests) into artifacts/native/032-demo-frame-qa, with configurable scenario scope and capture cadence plus a documented human QA checklist.

## Plan

1. Capture pipeline
   - Add a demo frame export helper that writes BMP images from `engine_get_frame_buffer` (RGBA) and optional UI overlay data.
   - Gate exports behind `BANANA_DEMO_FRAME_EXPORT=1` with cadence sourced from `BANANA_DEMO_FRAME_INTERVAL` or the existing snapshot interval.
2. Bundle metadata
   - Emit `manifest.json` per run and per-frame metadata JSON including scenario, tick, timestamp, scene catalog entry, and run identity.
   - Surface export failures via stderr and non-zero exit when export is enabled.
3. Orchestration and artifacts
   - Update `run-native-feedback-loop.sh`, `run-war-test-suites.sh`, and `refresh-coherent-world-evidence.sh` to pass export settings and place bundles under artifacts/native/032-demo-frame-qa.
4. QA workflow
   - Add a review checklist and review record template under contracts, referenced by quickstart.
5. Validation
   - Add a Windows-only CTest smoke to run `banana_engine_win32_dx12_poc` autotest mode with export enabled and verify a bundle exists.
   - Ensure non-export test runs remain unchanged.

## Technical Context

**Language/Version**: C11 (native runtime/tests), Bash (orchestration scripts)

**Primary Dependencies**: CMake 3.21+, DX12 POC runtime (`banana_engine_win32_dx12_poc`), native feedback loop factory

**Storage**: Local filesystem artifacts under `artifacts/native/032-demo-frame-qa/` (frame bundles plus metadata)

**Testing**: CTest native targets, `scripts/run-native-feedback-loop.sh`, `scripts/run-war-test-suites.sh`

**Target Platform**: Windows 10+ (DX12 runtime); scripts run via Bash on Windows

**Project Type**: Native runtime and test harness with artifact packaging

**Performance Goals**: Export frames at configured cadence without altering simulation outcomes; keep export overhead bounded to the capture interval

**Constraints**: Export optional; missing frames must fail export-enabled runs; avoid new external image dependencies; avoid expanding large files without a split plan

**Scale/Scope**: Demo scene catalog variants and feedback-loop scenarios (warfront/negotiate/comeback)

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- [x] Player trust and disclosure alignment verified (internal QA evidence only; no storefront claims touched).
- [x] Storefront governance artifacts identified when public Steam copy is affected (not applicable).
- [x] Cross-domain contracts mapped for touched layers (native runtime, test orchestration, artifact docs).
- [x] Quality gates defined with measurable checks for deterministic behavior, integration paths, and failure handling.
- [x] Reproducible delivery path identified for target runtime channels and evidence artifacts listed for release validation.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/032-demo-frame-qa/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   └── demo-frame-qa.md
└── tasks.md
```

### Source Code (repository root)

```text
src/native/engine/
├── win32_dx12_poc/app/main.c
├── win32_dx12_poc/scene/demo_scene_catalog.c
├── render/renderer.h
├── runtime/engine/engine_host.h
tests/native/feedback/
├── native_feedback_loop_factory.c
├── scripts/*.dx12play
scripts/
├── run-native-feedback-loop.sh
├── run-war-test-suites.sh
└── refresh-coherent-world-evidence.sh
artifacts/native/032-demo-frame-qa/
```

**Structure Decision**: Keep changes in native runtime and test harness, reuse existing scripts and artifacts, and avoid introducing a new service layer.

## Storefront Deliverables

Not applicable (no public Steam copy or storefront assets change).
