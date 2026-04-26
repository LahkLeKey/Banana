# 041 Compose-Orchestrated Run Profiles

Status: COMPLETE

## Summary

Establishes Docker Compose as the canonical runtime orchestrator for Banana local run profiles so startup/shutdown/ready-state behavior is deterministic and reproducible across contributor machines.

## Execution Tracker

- [x] Added spec, checklist, plan, research, data model, contract, quickstart, and tasks artifacts.
- [x] Added foundational canonical orchestration scripts for profile bring-up/down/readiness.
- [x] Routed primary runtime wrappers and WSL2 launchers through canonical profile entrypoints.
- [x] Finalized onboarding/runtime docs and troubleshooting parity updates.
- [x] Added repeat-run reproducibility validator and compose CI/runtime contract integration hooks.
- [x] Completed close-out validation lane and recorded reproducibility evidence.

## Validation Evidence

- `python scripts/validate-ai-contracts.py` returned no issues.
- `bash scripts/validate-spec-tasks-parity.sh --all` returned `OK` for `041-compose-orchestrated-run-profiles` and all existing specs.
- Primary profile lifecycle validated with canonical entrypoints:
	- `bash scripts/compose-run-profile.sh --profile runtime --action up`
	- `bash scripts/compose-profile-ready.sh --profile runtime --timeout-sec 120`
	- `bash scripts/compose-run-profile.sh --profile runtime --action down`
- Secondary profile lifecycle validated with canonical entrypoints:
	- `bash scripts/compose-run-profile.sh --profile apps --action up`
	- `bash scripts/compose-profile-ready.sh --profile apps --timeout-sec 180`
	- `bash scripts/compose-run-profile.sh --profile apps --action down`
- Reproducibility validation passed:
	- `bash scripts/validate-compose-run-profiles.sh --profile runtime --attempts 1`
	- artifact: `.artifacts/compose-repro/runtime/reproducibility-result.json`

## Notes

- Canonical script surfaces are `scripts/compose-run-profile.sh` and `scripts/compose-profile-ready.sh`.
- Existing wrapper scripts remain available as compatibility entrypoints while delegating to canonical orchestration.
