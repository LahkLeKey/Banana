# Confidence Orchestration Guidance

## Residual Risks

- Confidence scoring remains operator-estimated; inconsistent calibration across operators can cause drift.
- Extension health can still be partially obscured by third-party CLI rendering bugs; registry fallback reduces but does not eliminate unknown unknowns.
- Rapid context switching between shards can inflate perceived confidence if heartbeat entries are not updated at each boundary.

## Operator Guidance

- Log heartbeat entries for every major transition, not just successful completions.
- Use `spec-confidence-gate.sh` before edits, broad command dispatch, or shard-plan transitions.
- When confidence is below 80%, capture a short human decision record before proceeding.
- Run `spec-extension-preflight.sh --update-first` at orchestration start and when a workflow capability changes.
