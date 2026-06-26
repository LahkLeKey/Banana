# Banana CLI (Python Scaffold)

This package scaffolds the root-level Banana CLI under `cli/*` with K3H4-focused command routing.

Current implementation status:

- command router is implemented
- `k3h4` subcommands are scaffolded stubs: sample, run, explain, export, doctor
- native-direct execution is planned for follow-up stories

## Local usage (no install)

From `cli/banana`:

```bash
python -m banana_cli --help
python -m banana_cli k3h4 --help
```

## Install editable script entrypoint

From repo root:

```bash
python -m pip install -e cli/banana
banana --help
banana k3h4 --help
```
