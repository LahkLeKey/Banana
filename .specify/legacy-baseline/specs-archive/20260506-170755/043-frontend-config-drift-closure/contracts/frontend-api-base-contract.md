# Contract: Frontend API Base Resolution and Drift Diagnostics

## Contract Scope

This contract defines the required API-base configuration and resolution behavior for frontend slices launched via canonical Banana compose/task flows.

## Slice Mapping

| Slice | Runtime variable consumed | Override variable | Default | Effective expectation |
|---|---|---|---|---|
| React web | `VITE_BANANA_API_BASE_URL` | `BANANA_REACT_API_URL` | `http://localhost:8080` | UI must render non-empty API base |
| Electron desktop | `BANANA_API_BASE_URL` | `BANANA_ELECTRON_API_URL` | `http://localhost:8080` | Preload bridge `apiBaseUrl` must be non-empty |
| React Native web | `EXPO_PUBLIC_BANANA_API_BASE_URL` | `BANANA_MOBILE_API_URL` | `http://localhost:8080` | Web preview runtime must resolve non-empty API base |

## Resolution Rules

1. Canonical startup path MUST run runtime readiness before frontend slice readiness.
2. Frontend effective API base MUST resolve to non-empty value at runtime.
3. If effective API base resolves empty, UI/runtime MUST emit explicit configuration error guidance.
4. Diagnostics MUST identify failing layer: launch path, compose interpolation, container env, or effective runtime.

## Validation Requirements

Minimum validation for contract pass:

1. Compose interpolation includes expected keys and values for target slice.
2. Running container env exposes expected resolved key.
3. Effective runtime value in frontend process is non-empty.
4. Rendered UI (or bridge output for Electron) confirms non-empty API base.

## Failure Semantics

A contract failure occurs when any of the following is true:

- Canonical launch sequence is bypassed for target slice.
- Required env key is missing or resolves empty.
- Effective runtime resolution returns empty string.
- UI reports `API base: <unset>` during healthy startup.

## Remediation Baseline

1. Stop apps and runtime profiles.
2. Remove stale compose services/orphans for affected profile.
3. Relaunch via canonical compose profile tasks.
4. Re-run validation checks in order (interpolation -> container env -> effective runtime -> rendered output).
