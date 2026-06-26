# Banana CLI (Python Scaffold)

This package scaffolds the root-level Banana CLI under `cli/*` with K3H4-focused command routing.

Current implementation status:

- command router is implemented
- `k3h4 sample` emits deterministic canonical JSON datasets (`schema_version=1`)
- `k3h4` remaining subcommands are scaffolded: run, explain, export
- `k3h4 doctor` runs read-only preflight diagnostics for Python, native ABI, and schema checks
- native-direct execution is planned for follow-up stories

## Sample dataset generation

`banana k3h4 sample` emits canonical JSON to stdout and is designed for stdin piping into `banana k3h4 run`.

Flags:

- `--seed` (default `42`)
- `--count` (default `4`, must be `>=1`)
- `--dims` (default `16`, must be `1..16`)
- `--preset` (`baseline` or `combat`)

Determinism contract:

- Same args and seed produce byte-identical output.
- Output is emitted with stable key ordering.

Golden sample examples:

```bash
python -m banana_cli k3h4 sample --seed 7 --count 2 --dims 16 --preset baseline
python -m banana_cli k3h4 sample --seed 7 --count 2 --dims 16 --preset baseline | python -m banana_cli k3h4 run
```

## Doctor preflight checks

`banana k3h4 doctor` validates:

- Python runtime compatibility (`>=3.10`)
- native library resolution chain:
	1. `--native-lib`
	2. `BANANA_NATIVE_PATH`
	3. autodiscovery under `out/v3-native`, `out/native`, then `out`
- native ABI handshake (`banana_native_v3_abi_version`, `banana_native_v3_ping`)
- canonical schema file presence/version (`src/typescript/api/coverage-denominator.json`, `schema_version=1`)

On failures, strict mode emits JSON error envelopes to stderr (`error_code`, `message`, `field_path`) and exits non-zero.

Linux is the V1 support baseline. Non-Linux platforms are reported as experimental.

### Golden examples

From repo root:

```bash
python -m banana_cli k3h4 doctor
```

With explicit native library override:

```bash
python -m banana_cli k3h4 doctor --native-lib out/v3-native/Debug/banana_native.dll
```

## Local usage (no install)

From `cli/banana`:

```bash
python -m banana_cli --help
python -m banana_cli k3h4 --help
python -m banana_cli k3h4 sample --help
python -m banana_cli k3h4 doctor --help
```

## Install editable script entrypoint

From repo root:

```bash
python -m pip install -e cli/banana
banana --help
banana k3h4 --help
```
