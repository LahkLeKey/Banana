# Research: Demo Frame QA Evidence

## Decision 1: Capture entry points
Decision: Use the DX12 POC autotest path for demo scene frame capture and the native feedback loop factory for war-suite runs, both emitting frame bundles via a shared export helper.
Rationale: These runners already own demo scene selection (BANANA_DX12_POC_SCENE_VARIANT) and scenario suites (run-war-test-suites.sh), so capture stays aligned with existing test workflows.
Alternatives considered: A new standalone renderer CLI (rejected: duplicates POC scene selection and would need new orchestration).

## Decision 2: Image format
Decision: Write BMP images from RGBA frame buffers.
Rationale: BMP can be written with standard C (no third-party image libs) and is viewable on Windows, keeping dependencies minimal.
Alternatives considered: PNG via stb/lodepng (rejected: new third-party dependency and larger surface); raw RGBA only (rejected: not review-friendly).

## Decision 3: Cadence and scope controls
Decision: Reuse existing suite or scenario selection and snapshot interval as the default capture cadence, with an optional BANANA_DEMO_FRAME_INTERVAL override.
Rationale: Keeps configuration aligned to current test harness knobs while adding an explicit override for QA runs.
Alternatives considered: A new dedicated capture schedule file (rejected: adds a new config surface).

## Decision 4: Bundle layout and failure handling
Decision: Emit a run-scoped bundle with manifest.json, per-frame metadata JSON, and per-frame image files; fail the run when export is enabled and a frame write fails.
Rationale: Matches existing artifact patterns under artifacts/native and ensures missing evidence is visible.
Alternatives considered: Embedding metadata only in log output (rejected: makes QA review and automation harder).
