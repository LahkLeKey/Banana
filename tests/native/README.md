# Native Feedback Loop Tests

The legacy native unit-test fleet has been retired in favor of one scenario-driven
feedback loop factory:

- `feedback/native_feedback_loop_factory.c`

Run paths:

- Single loop (legacy scenario mode): `bash scripts/run-native-feedback-loop.sh --scenario warfront`
- Single loop (DX12 playloop script mode): `bash scripts/run-native-feedback-loop.sh --script tests/native/feedback/scripts/warfront.dx12play --non-interactive`
- Single loop (DX12 scaffold-only smoke): `bash scripts/run-native-feedback-loop.sh --script tests/native/feedback/scripts/dx12-client-scaffold.dx12play --non-interactive`
- Suite loops: `bash scripts/run-war-test-suites.sh --suite evidence --non-interactive`
- Suite loops with DX12 playloop scripts: `bash scripts/run-war-test-suites.sh --suite evidence --non-interactive --script-dir tests/native/feedback/scripts`
- Evidence refresh: `bash scripts/refresh-coherent-world-evidence.sh`
- CTest smoke targets: `banana_native_feedback_loop_factory_smoke` (hold-line gameplay), `banana_native_feedback_loop_factory_negotiate_smoke`, `banana_native_feedback_loop_factory_comeback_smoke`, `banana_native_feedback_loop_factory_flank_smoke`, and `banana_native_feedback_loop_factory_dx12_scaffold_smoke` (scaffold lifecycle)

Demo frame export (visual QA evidence):

- Enable via `BANANA_DEMO_FRAME_EXPORT=1` (optional cadence: `BANANA_DEMO_FRAME_INTERVAL=<ticks>`)
- Output root defaults to `artifacts/native/032-demo-frame-qa/runs` (override with `BANANA_DEMO_FRAME_OUTPUT_DIR`)
- Optional labels: `BANANA_DEMO_FRAME_SUITE=<name>` and `BANANA_DEMO_FRAME_RUN_LABEL=<label>`
- Validate latest bundle: `bash tests/native/feedback/validate_demo_frame_bundle.sh --output-dir artifacts/native/032-demo-frame-qa/runs`

DX12 playloop fixture scripts:

- `feedback/scripts/warfront.dx12play`
- `feedback/scripts/dx12-client-scaffold.dx12play`
- `feedback/scripts/negotiate.dx12play`
- `feedback/scripts/comeback.dx12play`
- `feedback/scripts/flank.dx12play`

Playloop command surface (Playwright-style) includes gameplay actions like
`page.goto`, `page.reload`, `actor.spawn`, `state.sentience`, `engine.config`,
`page.tick`, plus scaffold client actions like `client.ping`,
`client.world.*`, `client.launch.decide`, `client.ui.*`, with `expect.*`
assertions over both gameplay and scaffold metrics.

The factory prints operator-readable telemetry snapshots so human review can drive
follow-up balancing and gameplay adjustments.
