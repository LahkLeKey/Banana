# Quickstart: Demo Frame QA Evidence

## Build (once per workspace)

1. Configure native build output:
   cmake -S src/native -B out/v3-native

2. Build the feedback loop factory and DX12 POC:
   cmake --build out/v3-native --target banana_native_feedback_loop_factory banana_engine_win32_dx12_poc

## Capture frames from war-suite demo tests

1. Run a demo suite with capture enabled:
   BANANA_DEMO_FRAME_EXPORT=1 BANANA_DEMO_FRAME_OUTPUT_DIR=artifacts/native/032-demo-frame-qa/runs \
   bash scripts/run-war-test-suites.sh --suite evidence --snapshot-interval 4 --script-dir tests/native/feedback/scripts --non-interactive

2. Review the bundle manifest:
   artifacts/native/032-demo-frame-qa/runs/<run-id>/manifest.json

## Capture frames from demo scene variants

1. Run a single variant in autotest mode:
   BANANA_DX12_POC_AUTOTEST=1 BANANA_DX12_POC_AUTOTEST_SECONDS=2 BANANA_DX12_POC_SCENE_VARIANT=0 \
   BANANA_DEMO_FRAME_EXPORT=1 BANANA_DEMO_FRAME_OUTPUT_DIR=artifacts/native/032-demo-frame-qa/runs \
   C:/Github/Banana/out/v3-native/engine/Debug/banana_engine_win32_dx12_poc.exe

2. Repeat for other variants as needed.

## QA review workflow

- Follow the checklist in .specify/specs/032-demo-frame-qa/contracts/demo-frame-qa.md.
- Copy the template from .specify/specs/032-demo-frame-qa/contracts/demo-frame-qa-review-record.md.
- Save review records under the bundle review/ folder.
