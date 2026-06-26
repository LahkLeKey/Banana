# Banana CLI (Scaffold)

This package hosts the root-level Banana CLI under `cli/*`.

Current scope is K3H4-only command scaffolding so iteration can happen from terminal workflows without frontend dependencies.

## Commands (Scaffold)

- `banana k3h4 sample`
- `banana k3h4 run`
- `banana k3h4 explain`
- `banana k3h4 export`

All commands are currently stubs and print help or a not-implemented placeholder.

## Local usage

From repo root:

```bash
bun run banana --help
bun run banana k3h4 --help
```

From package folder:

```bash
bun run banana --help
```