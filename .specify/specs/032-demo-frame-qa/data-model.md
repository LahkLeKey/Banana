# Data Model: Demo Frame QA Evidence

## FrameBundle
Fields:
- run_id: string (timestamp plus suite or scenario)
- created_at: ISO-8601 string
- suite: string
- scenarios: string[]
- capture_interval_ticks: number
- format: string (bmp)
- output_dir: string
- status: string (complete | partial | failed)
- frames: FrameRecord[]
- failures: FrameFailure[] (optional)

## FrameRecord
Fields:
- frame_id: string
- scenario: string
- tick: number
- timestamp_ms: number
- image_path: string
- metadata_path: string
- width: number
- height: number
- render_backend: string (dx12 | glfw | stub)
- ui_overlay: boolean
- scene_variant: number (optional)
- scene_key: string (optional)

## FrameFailure
Fields:
- frame_id: string
- scenario: string
- tick: number
- error: string

## CaptureContext
Fields:
- build_config: string
- build_dir: string
- script_path: string (optional)
- demo_scene_variant: number (optional)
- demo_scene_key: string (optional)
- git_sha: string (optional)
- os: string
- renderer_backend: string

## QAReviewRecord
Fields:
- review_id: string
- bundle_run_id: string
- reviewer: string
- reviewed_at: ISO-8601 string
- verdict: string (pass | fail | needs-followup)
- checklist_results: map<string, string>
- notes: string
- frame_refs: string[]

## Relationships
- FrameBundle has one CaptureContext and many FrameRecord entries.
- QAReviewRecord references one FrameBundle and optional FrameRecord IDs.

## Validation Rules
- run_id must be non-empty and unique per export run.
- capture_interval_ticks must be greater than 0.
- image_path and metadata_path must exist for each FrameRecord.
- tick values should be unique within a scenario.
