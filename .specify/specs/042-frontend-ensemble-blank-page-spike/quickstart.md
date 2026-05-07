# Quickstart: Ensemble Predict Blank Page UX Spike

## Goal

Reproduce, classify, and validate the ensemble "Predict" blank-page regression while preserving current baseline verdict UX behavior.

## Prerequisites

- Workspace root: `c:/Github/Banana`
- Bun dependencies installed for React app and shared UI package
- Runtime API channels available when running full app behavior checks

## Validation Flow

1. Bring up runtime dependencies and apps profile:

```bash
bash scripts/compose-run-profile.sh --profile runtime --action up -- --build
bash scripts/compose-profile-ready.sh --profile runtime
bash scripts/compose-run-profile.sh --profile apps --action up -- --build
bash scripts/compose-profile-ready.sh --profile apps
```

2. Reproduce primary trigger manually in browser:
- Open the React app.
- Enter ensemble input.
- Click Predict ensemble.
- Record whether outcome is navigation/reload blank or render blank.

3. Validate keyboard submit path:
- Trigger ensemble submit with Enter from the same form context.
- Compare behavior to button-click path.

4. Run focused frontend regression tests:

```bash
cd src/typescript/react
bun test src/App.test.tsx
```

5. Run broader script/spec safety checks for spike artifacts:

```bash
cd c:/Github/Banana
python scripts/validate-spec-tasks-parity.py .specify/specs/042-frontend-ensemble-blank-page-spike
```

6. Capture findings in spike artifacts:
- root-cause hypothesis
- submit/state guardrails
- regression test requirements

7. Tear down runtime profiles when done:

```bash
bash scripts/compose-run-profile.sh --profile apps --action down
bash scripts/compose-run-profile.sh --profile runtime --action down
```

## Expected Outputs

- Repro path documented for blank-page bug class.
- Clear distinction between navigation-reload and render-blank failure classes.
- Baseline-preservation checks defined for verdict/escalation/retry UX.
- Implementation-ready guardrails and regression test requirements.

## Execution Evidence (2026-04-26)

- `bun test src/App.test.tsx`: pass (13/13).
- `python scripts/validate-spec-tasks-parity.py .specify/specs/042-frontend-ensemble-blank-page-spike`: no output (no drift reported).
- Integrated browser checks at `http://localhost:5173/`:
	- click submit path stayed on same URL and rendered inline recoverable error when API was unavailable,
	- keyboard submit path (`Ctrl+Enter`) stayed on same URL and rendered inline recoverable error when API was unavailable.
