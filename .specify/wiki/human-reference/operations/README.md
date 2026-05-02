# Operations

> [Home](../Home.md) › Operations

Runbooks, CI notes, and runtime channel documentation.

## Pages In This Section

| Page | Description |
|------|-------------|
| [CI and Compose Notes](ci-compose-notes.md) | Workflow jobs and compose profiles explained |
| [WSL2 Runtime Channels](wsl2-runtime-channels.md) | Mobile and desktop channel launchers |
| [CI Runtime Compatibility](ci-runtime-compatibility.md) | Node version and Actions runtime guardrails |
| [Compose CI Stabilization](ci-compose-stabilization.md) | Failure evidence and triage artifacts |

## Fast Triage Path

1. Native build failure → `native-c-tests-coverage` job logs.
2. Compose health failure → `compose-runtime` logs and `/health` readiness.
3. Mobile launch failure → WSL2 distro check, WSLg preflight, Android SDK.
