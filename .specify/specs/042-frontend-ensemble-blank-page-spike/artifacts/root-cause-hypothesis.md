# Root-Cause Hypothesis (US3)

## Objective

Summarize likely causes for the Predict ensemble blank-page behavior and bound confidence for implementation planning.

## Hypothesis H1 (Primary)

- Statement: Native form submit defaults diverged across trigger paths and could perform browser navigation when submit handling was not uniformly guarded.
- Confidence: High
- Evidence:
  - Guarded click and keyboard submit paths now remain location-stable.
  - Explicit prevention of native submit behavior plus button type=button removed navigation-reload symptoms in validation.

## Hypothesis H2 (Secondary)

- Statement: A render-time failure could present as a blank-screen symptom even when no browser navigation occurs.
- Confidence: Medium
- Evidence:
  - Render-blank class remains possible in theory but was not observed in this session.
  - Current implemented classification covers navigation detection; render-failure diagnostics remain an extension surface.

## Decision For Implementation

- Prioritize H1 remediation as the active fix path.
- Keep H2 tracked as residual risk and add regression diagnostics to quickly separate navigation from render failures.

## Out-of-Scope Confirmations

- No backend model contract change required for this UX slice.
- No native interop contract change required for this UX slice.
