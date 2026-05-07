# Wave 0 Execution Backlog

Scope source: spikes 186-200
Target page: https://banana.engineer/data-science

## Slice A: Output Contract Normalization
- Implement canonical static output payload shape in Data Science renderer.
- Add legacy-to-canonical normalization shims with explicit fallback messages.
- Evidence: before/after notebook output snapshots and parser tests.

## Slice B: Layout and Readability Baseline
- Replace mixed ad hoc output sizing with deterministic rich-output layout rules.
- Improve report-mode readability for markdown, table, and chart flow.
- Evidence: desktop/mobile screenshots and layout behavior notes.

## Slice C: Persistence and Export Baseline
- Harden notebook output persistence across refresh/import/export.
- Ensure static outputs survive round-trip serialization.
- Evidence: import/export test notebook and round-trip verification log.

## Slice D: Error and Degraded-State Behavior
- Implement user-facing fallback surfaces for unsupported/broken visual payloads.
- Ensure recovery affordances are visible and non-blocking.
- Evidence: forced-failure scenarios and fallback screenshot set.

## Slice E: Report/Print Path
- Add print/PDF-friendly output behavior for report-style notebooks.
- Validate readable page breaks and static chart fidelity.
- Evidence: print preview captures and generated PDF comparison.

## Wave 0 Exit Checklist
- All slices A-E implemented and validated.
- Wave 0 tasks file updated to completed with evidence links.
- Follow-up readiness packet created for Wave 1 handoff.
