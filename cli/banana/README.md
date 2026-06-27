# Banana CLI (Python Scaffold)

This package scaffolds the root-level Banana CLI under `cli/*` with K3H4-focused command routing.

Current implementation status:

- command router is implemented
- `k3h4 sample` emits deterministic canonical JSON datasets (`schema_version=1`)
- `k3h4 dialogue-sample` emits deterministic single-turn dialogue fixture JSON (`schema_version=1`)
- `k3h4 run` executes native-direct K3H4 pipeline via `ctypes` and canonical stdin JSON
- `k3h4 dialogue-run` executes deterministic dialogue turn routing through native full-chain symbol binding
- `k3h4 dialogue-export` emits machine-consumable dialogue turn artifacts in canonical JSON or CSV
- `k3h4 explain` interprets run output with field-referenced analysis records
- `k3h4 export` emits machine-consumable artifacts in canonical JSON or CSV
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

## Run pipeline execution

`banana k3h4 run` executes native `banana_native_v3_netcode_build_k3h4` for each sample entry.

Input contract:

- Primary path: canonical JSON from stdin (`sample | run`)
- Optional path: `--input-file`
- Requires exact native-aligned field names in each sample object (no aliases in V1)
- Requires `schema_version=1`

Native library resolution precedence:

1. `--native-lib`
2. `BANANA_NATIVE_PATH`
3. autodiscovery under `out/v3-native`, `out/native`, then `out`

Output contract:

- Canonical JSON to stdout with stable key ordering
- `schema_version=1`
- Validated against checked-in schema: `cli/banana/schema/k3h4-run-output.v1.json`

Strict diagnostics:

- Validation/contract failures exit non-zero
- Strict mode emits JSON error envelopes on stderr (`error_code`, `message`, `field_path`)

Platform policy:

- Linux is the V1 support baseline
- Windows and macOS are experimental in V1

Golden run examples:

```bash
python -m banana_cli k3h4 sample --seed 7 --count 2 --dims 16 --preset baseline | python -m banana_cli k3h4 run
python -m banana_cli k3h4 run --input-file sample.json --native-lib out/v3-native/Debug/banana_native.dll
```

## Dialogue fixture generation and preflight

`banana k3h4 dialogue-sample` emits canonical single-turn dialogue fixture JSON for local iteration.

Flags:

- `--seed` (default `42`)
- `--preset` (`pilot-edda` or `hard-block-self-harm`)

`banana k3h4 dialogue-run` validates fixture shape and executes one deterministic dialogue turn through the native full-chain symbol.

Input contract:

- Primary path: canonical fixture JSON from stdin (`dialogue-sample | dialogue-run`)
- Optional path: `--input-file`
- Required fields: `npc_id`, `quest_state_id`, `region_id`, `intent_id`, `policy_context`, `prior_memory_delta`
- Requires `schema_version=1`
- Validated against checked-in input schema: `cli/banana/schema/k3h4-dialogue-run-input.v1.json`

Current output contract:

- canonical JSON with `fixture_hash`, decision metadata, mutation flags, renderable line candidate, observability, native resolution metadata, and `status=executed`
- Validated against checked-in output schema: `cli/banana/schema/k3h4-dialogue-run-output.v1.json`
- Taxonomy invariants:
	- `response_policy=hard_block` requires `deny_reason_code` in reserved range `9100..9199`, both mutation guards true, and `memory_delta_applied=0`
	- `response_policy=safe_redirect` requires both mutation guards true and deny code outside `9100..9199`

Schema override flags:

- `--schema-path`: dialogue-run input schema path
- `--output-schema-path`: dialogue-run output schema path

Golden dialogue examples:

```bash
python -m banana_cli k3h4 dialogue-sample --seed 11 --preset pilot-edda \
	| python -m banana_cli k3h4 dialogue-run --native-lib out/v3-native/Debug/banana_native.dll

python -m banana_cli k3h4 dialogue-sample --seed 11 --preset hard-block-self-harm > fixture.json
python -m banana_cli k3h4 dialogue-run --input-file fixture.json --native-lib out/v3-native/Debug/banana_native.dll
```

## Dialogue export artifacts

`banana k3h4 dialogue-export` exports dialogue-run output into machine-consumable JSON or CSV.

Input and strict behavior:

- Primary path: dialogue-run output from stdin (`dialogue-sample | dialogue-run | dialogue-export`)
- Optional path: `--input-file`
- Strict mode defaults on and returns non-zero on invalid input
- Input validated against checked-in schema: `cli/banana/schema/k3h4-dialogue-run-output.v1.json`
- Dialogue-export also enforces dialogue-run taxonomy invariants before writing artifacts

Output formats:

- `--format json` (default): canonical JSON with `schema_version=1`
- `--format csv`: single-row artifact summary
- Export output validated against checked-in schema: `cli/banana/schema/k3h4-dialogue-export-output.v1.json`

Schema override flags:

- `--input-schema-path`: dialogue-export input schema path
- `--schema-path`: dialogue-export output schema path

Golden dialogue-export examples:

```bash
python -m banana_cli k3h4 dialogue-sample --seed 11 --preset pilot-edda \
	| python -m banana_cli k3h4 dialogue-run --native-lib out/v3-native/Debug/banana_native.dll \
	| python -m banana_cli k3h4 dialogue-export

python -m banana_cli k3h4 dialogue-run --input-file fixture.json --native-lib out/v3-native/Debug/banana_native.dll \
	| python -m banana_cli k3h4 dialogue-export --format csv --output-file artifacts/dialogue-turn.csv
```

## Explain interpretation

`banana k3h4 explain` transforms run output into concrete, field-referenced interpretation records.

Input contract:

- Primary path: run output from stdin (`sample | run | explain`)
- Optional path: `--input-file`
- Strict validation defaults to on and returns non-zero on invalid input

Coverage of explanations:

- convergence and observability fields
- radius state interpretation
- weighted assignment validity/score interpretation
- spectral proxy state/frequency interpretation

Golden explain example:

```bash
python -m banana_cli k3h4 sample --seed 11 --count 1 --dims 16 --preset baseline \
	| python -m banana_cli k3h4 run --native-lib out/v3-native/Debug/banana_native.dll \
	| python -m banana_cli k3h4 explain
```

## Export artifacts

`banana k3h4 export` produces machine-consumable artifacts for AI loops.

Input and strict behavior:

- Primary path: run output from stdin (`sample | run | export`)
- Optional path: `--input-file`
- Strict mode defaults on and returns non-zero on invalid input
- Under strict failures, no partial artifact files are written

Output formats:

- `--format json` (default): canonical JSON with `schema_version=1`
- `--format csv`: tabular summary rows for each run result

Metadata envelope includes:

- deterministic hash derived from canonical run payload
- run configuration (sample count and native resolution path/source)

Canonical export schema:

- `cli/banana/schema/k3h4-export-output.v1.json`

Golden export examples:

```bash
python -m banana_cli k3h4 sample --seed 11 --count 1 --dims 16 --preset baseline \
	| python -m banana_cli k3h4 run --native-lib out/v3-native/Debug/banana_native.dll \
	| python -m banana_cli k3h4 export

python -m banana_cli k3h4 sample --seed 11 --count 1 --dims 16 --preset baseline \
	| python -m banana_cli k3h4 run --native-lib out/v3-native/Debug/banana_native.dll \
	| python -m banana_cli k3h4 export --format csv --output-file artifacts/k3h4-export.csv
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

## Dialogue taxonomy smoke script

Run the local dialogue taxonomy smoke checks (standard pass, hard-block pass, fail-closed invalid deny code):

```bash
bash cli/banana/scripts/run-dialogue-taxonomy-smoke.sh
```

Optional explicit native library path:

```bash
bash cli/banana/scripts/run-dialogue-taxonomy-smoke.sh out/v3-native/Debug/banana_native.dll
```

## Local usage (no install)

From `cli/banana`:

```bash
python -m banana_cli --help
python -m banana_cli k3h4 --help
python -m banana_cli k3h4 sample --help
python -m banana_cli k3h4 dialogue-sample --help
python -m banana_cli k3h4 run --help
python -m banana_cli k3h4 dialogue-run --help
python -m banana_cli k3h4 dialogue-export --help
python -m banana_cli k3h4 explain --help
python -m banana_cli k3h4 export --help
python -m banana_cli k3h4 doctor --help
```

## Install editable script entrypoint

From repo root:

```bash
python -m pip install -e cli/banana
banana --help
banana k3h4 --help
```
