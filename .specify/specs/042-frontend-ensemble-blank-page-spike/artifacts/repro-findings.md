# Repro Findings: Ensemble Predict Blank-Page Regression (US1)

## Scope

- Feature: 042 frontend ensemble blank-page spike
- Story: US1 (prevent reload/blank transition on Predict ensemble)
- Date: 2026-04-26

## Classification Summary

- Primary class observed in current session: navigation_reload risk addressed by submit guardrails.
- Secondary class observed in current session: render_blank not observed.

## Evidence

### Automated

- Command: bun test src/App.test.tsx
- Result: pass (11/11)
- Added checks:
  - click submit keeps location stable and renders verdict
  - keyboard submit keeps location stable and renders verdict

### Integrated Browser

- URL before click submit: http://localhost:5173/
- URL after click submit: http://localhost:5173/
- Outcome: no navigation/reload; inline recoverable error displayed when API is unreachable.

- URL before keyboard submit: http://localhost:5173/
- URL after keyboard submit: http://localhost:5173/
- Outcome: no navigation/reload; inline recoverable error displayed when API is unreachable.

## Root-Cause Hypothesis (US1 slice)

- The highest-risk vector for reload behavior is default/native submit semantics diverging across trigger paths.
- Hardening submit handling for both form submit and button click, plus explicit keyboard trigger handling, removes this vector.

## Guardrails Implemented

- Explicit native submit suppression via preventDefault + stopPropagation.
- Click path decoupled from native submit by using button type=button.
- Keyboard submit path made explicit via Ctrl+Enter handler.
- Location snapshot comparison added to classify unexpected navigation transitions during submit lifecycle.

## Residual Risk

- A runtime crash path could still produce a visual blank state unrelated to browser navigation.
- This is tracked for subsequent spike phases focused on render-blank diagnostics and baseline-preservation expansion.
