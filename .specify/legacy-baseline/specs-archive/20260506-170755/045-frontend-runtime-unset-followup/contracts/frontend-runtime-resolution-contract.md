# Contract: Frontend Runtime Resolution and Recovery

## Purpose

Define required runtime behavior for API-base resolution, configuration-error rendering, and session bootstrap recovery.

## Runtime Inputs

- Web env key: `VITE_BANANA_API_BASE_URL`
- Electron preload key: `BANANA_API_BASE_URL`
- Canonical startup/remediation path: compose runtime/apps profile flow

## Resolution Contract

1. Frontend must normalize candidate API-base values before use.
2. If a valid effective API base exists, UI must render concrete API base value.
3. If no valid effective API base exists, UI must enter configuration error state with remediation guidance.

## Error Classification Contract

- Configuration errors are reserved for API-base resolution failures.
- Bootstrap/API transport failures must surface distinct error types/messages.
- Error messaging must include clear-condition semantics for recovery.

## Recovery Contract

1. Run canonical compose remediation path.
2. Reload frontend surface in integrated browser.
3. Re-evaluate resolution state.
4. If resolved, configuration error state clears and bootstrap continues.

## Closure Evidence Contract

- Evidence must include rendered API-base state after startup.
- Evidence must include whether configuration error rendered and why.
- Evidence must include post-remediation result for previously unresolved state.
