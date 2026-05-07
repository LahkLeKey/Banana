# Quickstart: Compose CI Stabilization

This guide validates the behavior required by spec `002-compose-ci-stabilization`.

## 1. Prerequisites

- Docker Engine with Docker Compose v2
- Bash shell on repository root
- .NET 8 SDK (for e2e project execution when present)
- Bun 1.x for TypeScript API surfaces when relevant

## 2. Validate compose lane entry points locally

Run each lane entry point from repository root:

```bash
bash scripts/compose-tests.sh
```

```bash
bash scripts/compose-runtime.sh api
curl -fsS http://localhost:8080/health
```

```bash
bash scripts/compose-electron.sh
```

After runtime checks, clean up:

```bash
docker compose --profile tests down -v --remove-orphans || true
docker compose --profile runtime down -v --remove-orphans || true
docker compose --profile electron down -v --remove-orphans || true
```

## 3. Validate e2e smoke bootstrap diagnostics path

```bash
bash scripts/compose-e2e-bootstrap.sh
curl -fsS http://localhost:8080/health
bash scripts/compose-e2e-teardown.sh
```

Expected: `.artifacts/e2e/bootstrap.json` is generated during bootstrap.

## 4. Run workflow-level verification

Trigger the Compose CI workflow and inspect lane outcomes:

```bash
gh workflow run "Compose CI"
```

After completion, verify:

- Every merge-gated lane has terminal `pass`, `fail`, or `skip` state with explicit reason.
- Failure or skip states include lane-scoped diagnostics artifact content.
- Artifact upload steps do not fail with missing-path errors.

## 5. Check runtime deprecation drift

Inspect workflow logs for runtime deprecation warnings:

```bash
gh run list --workflow "Compose CI" --limit 1
```

```bash
gh run view <run-id> --log | rg -i "node|deprecat"
```

Expected: no unowned Node runtime deprecation warnings in merge-gated workflows.

## 6. Evidence checklist

Capture and attach these outputs during implementation PR validation:

- Lane terminal summary per compose job
- Lane diagnostics bundle paths
- Artifact publication records (including skip/fallback behavior)
- Node runtime compatibility status (upgraded or exception documented)
