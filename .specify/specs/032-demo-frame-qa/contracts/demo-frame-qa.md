# Contract: Demo Frame QA Evidence

## Purpose
Define how demo-frame bundles are exported from demo test runs and how QA reviews them.

## Enablement
Frame export is disabled by default. Enable by setting:
- BANANA_DEMO_FRAME_EXPORT=1
- BANANA_DEMO_FRAME_OUTPUT_DIR=<path> (default: artifacts/native/032-demo-frame-qa/runs)
- BANANA_DEMO_FRAME_INTERVAL=<ticks> (optional; default: snapshot interval)
- BANANA_DEMO_FRAME_FORMAT=bmp (optional; default: bmp)

## Scope and cadence
- Scenario scope is controlled by existing test selectors:
  - scripts/run-war-test-suites.sh --suite or --scenario
  - BANANA_DX12_POC_SCENE_VARIANT for demo scene capture
- Capture cadence uses BANANA_DEMO_FRAME_INTERVAL or the test snapshot interval.

## Output layout

artifacts/native/032-demo-frame-qa/runs/<run-id>/
  manifest.json
  frames/<scenario>/
    <tick>-<timestamp>.bmp
    <tick>-<timestamp>.json
  review/
    review-record.md

Review records use the template at:
.specify/specs/032-demo-frame-qa/contracts/demo-frame-qa-review-record.md

## Manifest schema (example)

{
  "run_id": "2026-06-03T120000Z-evidence",
  "created_at": "2026-06-03T12:00:00Z",
  "suite": "evidence",
  "scenarios": ["warfront"],
  "capture_interval_ticks": 4,
  "format": "bmp",
  "frames": [
    {
      "frame_id": "warfront-8-123456",
      "scenario": "warfront",
      "tick": 8,
      "timestamp_ms": 123456,
      "image_path": "frames/warfront/8-123456.bmp",
      "metadata_path": "frames/warfront/8-123456.json",
      "scene_variant": 0,
      "scene_key": "port-koba"
    }
  ],
  "failures": []
}

## Failure handling
- If BANANA_DEMO_FRAME_EXPORT=1 and any frame write fails, the run exits non-zero and manifest.json includes a failure record.

## Retention and cleanup
- Retain bundles under artifacts/native/032-demo-frame-qa/runs for at least 30 days or until release sign-off.
- Cleanup older bundles during evidence refresh runs once retention requirements are met.
- Review records live inside each bundle under review/ and should remain with the bundle they reference.

## QA review checklist
- Verify the bundle exists and opens within 2 minutes.
- Confirm each frame has scenario, tick, timestamp, and run id metadata.
- Compare frame sequence to expected scenario behavior and record deviations.
- Record verdict and notes in review-record.md (use the template) and link to the bundle run id.
