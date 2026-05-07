# F5 Contract Decision (G2 + G3 + G4)

## G2 -- Compound vs single

**Decision**: Ship both. Single-channel entries enable per-channel
iteration; compound entries (`API + <channel>`) cover the common
"boot the demo" case. `stopAll: true` keeps shutdown clean.

Rejected alternative: compound-only. Reason: contributors
iterating on API alone do not need a frontend booted.

## G3 -- Env-var seam

**Decision**: Hard-code defaults in `launch.json`
(`VITE_BANANA_API_BASE_URL`, `EXPO_PUBLIC_BANANA_API_BASE_URL`,
`BANANA_API_PORT`). Contributors can override via the standard
VS Code mechanism (workspace settings, per-folder `.env`, or
forking the launch entry).

Rejected alternatives:
- `.env.local`: adds a setup step on a fresh clone; defeats the
  zero-config F5 promise.
- VS Code `inputs`: prompts on every F5; high friction for the
  common case.

## G4 -- Container contract boundary

**Decision**: F5 is **host-side only**. `launch.json` does NOT
shell out to `scripts/launch-container-channels-with-wsl2-*.sh`.
The runtime contract docs (`.github/instructions/`,
`.github/ubuntu-wsl2-runtime-contract.md`) remain the canonical
reproducible runtime; F5 is documented as developer convenience.

Rejected alternative: VS Code `tasks.json` wrappers around the
container scripts. Reason: those scripts are designed to run from
a Windows shell with Docker Desktop + Ubuntu WSL2 and surface
their own actionable error messages. Wrapping them in VS Code
tasks would dilute that contract without adding value.

## Acceptance signal

- Fresh clone -> install bun -> F5 `Frontends: API + React` ->
  `http://localhost:5173` boots and round-trips a verdict.
- Stopping the compound entry tears down both child processes.
