# Quickstart: Chat Bootstrap Fetch Failure Follow-up

## Goal

Verify that chat bootstrap no longer remains in `Failed to fetch` state under healthy startup, and that transport failures recover after runtime remediation.

## Prerequisites

- Workspace root: `c:/Github/Banana`
- Docker Desktop + Ubuntu WSL2 integration available
- Use VS Code integrated browser for rendered checks (constitution one-window requirement)

## Validation Steps

1. Bring up runtime profile first:

```bash
bash scripts/compose-run-profile.sh --profile runtime --action up -- --build
bash scripts/compose-profile-ready.sh --profile runtime
```

2. Bring up apps profile second:

```bash
bash scripts/compose-run-profile.sh --profile apps --action up -- --build
bash scripts/compose-profile-ready.sh --profile apps
```

3. Open `http://localhost:5173` in VS Code integrated browser.

4. Confirm healthy bootstrap path:
- No persistent `Chat bootstrap failed: Failed to fetch` banner.
- Bootstrap reaches ready state and chat controls become usable.

5. Simulate dependency loss (stop runtime only) and verify failure classification:

```bash
bash scripts/compose-run-profile.sh --profile runtime --action down
```

6. Reload app and confirm transport/bootstrap failure messaging is shown (not configuration-resolution messaging).

7. Recover runtime and verify bootstrap recovery:

```bash
bash scripts/compose-run-profile.sh --profile runtime --action up -- --build
bash scripts/compose-profile-ready.sh --profile runtime
```

8. Reload/retry app and confirm bootstrap returns to ready state without source edits.

9. Teardown after validation:

```bash
bash scripts/compose-run-profile.sh --profile apps --action down
bash scripts/compose-run-profile.sh --profile runtime --action down
```

## Expected Result

- Healthy startup produces chat-ready bootstrap state.
- Transport failures show distinct, actionable remediation guidance.
- After runtime recovery, bootstrap transitions from failure to ready deterministically.
