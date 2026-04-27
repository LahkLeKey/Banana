# Quickstart: One-Window Interactive Baseline

## Goal

Validate frontend/Electron behavior using a single VS Code workspace window.

## Steps

1. Start runtime profile:

```bash
bash scripts/compose-run-profile.sh --profile runtime --action up
bash scripts/compose-profile-ready.sh --profile runtime
```

2. Start apps profile:

```bash
bash scripts/compose-run-profile.sh --profile apps --action up -- --build
bash scripts/compose-profile-ready.sh --profile apps
```

3. Open the target app in VS Code integrated browser:

- React: `http://localhost:5173`
- Electron renderer channel: use the compose-backed renderer surface for current slice checks.

4. Perform rendered validation in the integrated browser:

- Confirm expected user-visible behavior for changed scope.
- For API-base contract checks, confirm the UI resolves a concrete API base and does not remain unset.

5. Capture closure evidence in the feature task context:

- Record the integrated browser URL/channel used.
- Record the rendered assertion outcome (pass/fail and what was observed).
- If remediation was needed, record the remediation command path used and post-remediation rendered result.

6. If behavior is unexpected, run diagnostics from the same workspace terminal:

```bash
bash scripts/validate-frontend-config-drift.sh --require-running
```

7. Teardown after validation:

```bash
bash scripts/compose-run-profile.sh --profile apps --action down
bash scripts/compose-run-profile.sh --profile runtime --action down
```

## Expected Outcome

- Startup, diagnostics, rendered checks, and teardown are completed without leaving VS Code.
- Task closure evidence includes rendered validation, not env/container checks alone.
