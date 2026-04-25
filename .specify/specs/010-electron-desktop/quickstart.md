# Quickstart — Electron Desktop (v2)

## Prerequisites

- Windows host with Docker Desktop + Ubuntu (Microsoft Store) WSL2 distro.
- WSLg + libGL available on the chosen distro.
- API reachable inside the container network.

## One-click launch (Windows)

```bash
scripts/launch-container-channels-with-wsl2-electron.sh
```

VS Code launch profile: **Banana Channels (Container Driven)**.

## Ubuntu/WSL2 entry

```bash
scripts/compose-electron-desktop-wsl2.sh
```

## Smoke channel (not the desktop UI)

```bash
docker compose run --rm electron-example npm run smoke
```

## Local fallback (non-container)

```bash
cd src/typescript/electron
npm ci
./run-desktop.sh
```

## Failure modes

- Exit code `42`: integration preflight failed; the message lists missing prerequisite(s).
- Distro order: set `BANANA_WSL_DISTRO=Ubuntu-24.04` to override; default falls back to `Ubuntu`.

## Scope guardrails

- Don't import from `@banana/ui/src/...` in renderer; import from package root only (`009`).
- Don't break exit code `42` semantics.
- Don't move smoke logic into the desktop UI channel.
