# Quickstart: Frontend Runtime Unset Follow-up

## Goal

Verify that Banana v2 no longer renders `API base: <unset>` under canonical startup and that session/chat readiness recovers after remediation.

## Steps

1. Start runtime profile:

```bash
bash scripts/compose-run-profile.sh --profile runtime --action up -- --build
bash scripts/compose-profile-ready.sh --profile runtime
```

2. Start apps profile:

```bash
bash scripts/compose-run-profile.sh --profile apps --action up -- --build
bash scripts/compose-profile-ready.sh --profile apps
```

3. Open `http://localhost:5173` in VS Code integrated browser.

4. Validate rendered runtime state:

- API base shows a concrete value and not `<unset>`.
- Configuration error banner is absent when API base is resolved.
- Session bootstrap progresses (no persistent `session: missing` dead-end once ready).

5. If unresolved state appears, execute remediation:

```bash
bash scripts/compose-run-profile.sh --profile apps --action down
bash scripts/compose-run-profile.sh --profile runtime --action down
bash scripts/compose-run-profile.sh --profile runtime --action up -- --build
bash scripts/compose-profile-ready.sh --profile runtime
bash scripts/compose-run-profile.sh --profile apps --action up -- --build
bash scripts/compose-profile-ready.sh --profile apps
```

6. Re-open integrated browser and re-check rendered state.

7. Run diagnostics while services are active:

```bash
bash scripts/validate-frontend-config-drift.sh --require-running
```

8. Teardown after verification:

```bash
bash scripts/compose-run-profile.sh --profile apps --action down
bash scripts/compose-run-profile.sh --profile runtime --action down
```

## Expected Outcome

- Canonical startup yields resolved API base text in UI.
- Error banner appears only for real configuration resolution failures.
- Session/chat path recovers to usable state after remediation.
