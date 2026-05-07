# Quickstart: Compose-Orchestrated Run Profiles

## 1. Validate baseline contract health

- python scripts/validate-ai-contracts.py
- bash scripts/validate-spec-tasks-parity.sh --all

## 2. Verify compose profile matrix coverage

- Confirm each supported local run profile maps to compose profiles and readiness checks.
- Confirm canonical start/stop entry points are documented and script-accessible.

## 3. Run primary profile through canonical orchestration

- Launch primary local development profile using the canonical compose-driven entry point.
- Verify required services reach expected ready state.

## 4. Run repeat determinism check

- Stop profile using canonical stop entry point.
- Relaunch profile using same entry point.
- Confirm readiness outcomes and failure signals are consistent.

Example command:

```bash
bash scripts/validate-compose-run-profiles.sh --profile runtime --attempts 3
```

## 5. Run secondary/optional profile checks

- Launch at least one additional supported profile (for example desktop/electron or tests profile) via compose orchestration path.
- Verify readiness and teardown behavior follows contract.

## 6. Validate troubleshooting guidance

- Simulate a prerequisite or dependency-health failure.
- Confirm troubleshooting mapping provides actionable remediation and consistent failure classification.
