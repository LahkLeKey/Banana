# Research: Ensemble Predict Blank Page UX Spike

## Decision 1: Treat blank page as two distinct failure classes

- Decision: Split investigation into (a) browser-level navigation/reload and (b) render-level blank state caused by runtime errors.
- Rationale: User-visible symptom is identical (blank page), but remediation and guardrails differ materially.
- Alternatives considered:
  - Treat all blank-page outcomes as navigation-only bugs (rejected: misses render-failure regressions).
  - Treat all outcomes as React render failures (rejected: misses form submit default navigation paths).

## Decision 2: Center the spike on submit semantics before downstream API behavior

- Decision: Prioritize ensemble form submit path analysis (button click + Enter key + pending state).
- Rationale: Reported trigger is clicking Predict ensemble, so submit wiring is the most probable and highest-value first slice.
- Alternatives considered:
  - Start with backend response handling (rejected: less likely to explain reload-style symptom).
  - Start with resilience queue/offline behavior (rejected: secondary path, not primary trigger wording).

## Decision 3: Preserve baseline UX contract during fix design

- Decision: Keep existing verdict copy, escalation panel behavior, retry affordance, and last-submitted sample continuity as non-regression contract.
- Rationale: Working baseline already exists and is user-visible value that should not be destabilized by this regression fix.
- Alternatives considered:
  - Rework ensemble UX in same slice (rejected: broad scope and higher regression risk).
  - Defer baseline checks until after implementation (rejected: weakens safety for this bugfix).

## Decision 4: Require deterministic regression coverage tied to the bug trigger

- Decision: Define regression tests that assert no full page navigation and continued in-page rendering on both success and recoverable error submits.
- Rationale: Without a deterministic check, this class of UX regression can reappear silently.
- Alternatives considered:
  - Manual-only QA checklist (rejected: non-deterministic and harder to enforce in CI).
  - Snapshot-only UI checks (rejected: may not detect navigation/reload triggers).

## Decision 5: Keep this spike frontend-only with no API contract change

- Decision: Document no required backend/native contract change in this feature.
- Rationale: Existing issue statement is interaction-specific and the baseline APIs already support verdict flow.
- Alternatives considered:
  - Introduce API-side safety toggles (rejected: unnecessary for reported symptom).
  - Route predict through alternative endpoint as workaround (rejected: scope creep and governance noise).
