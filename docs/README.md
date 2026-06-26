## Banana Runtime Overview

This repository includes a K3H4-first netcode analytics pipeline that flows across native, API, and frontend layers.

## CLI Quickstart (Python)

Root CLI scaffolding now lives under `cli/*` with a Python entrypoint in `cli/banana`.

From `cli/banana`:

```bash
python -m banana_cli --help
python -m banana_cli k3h4 --help
```

## K3H4 Model

K3H4 is the authoritative analytics model used for netcode clustering, convergence scoring, and ABI coverage reporting in the runtime UI.

- Theory notes for issue 1157: [docs/k3h4-lambda-hypersphere-notes.md](docs/k3h4-lambda-hypersphere-notes.md)

### Contract Surface

- Native: deterministic K3H4 metrics and ABI envelope metadata are produced from the native runtime/netcode layer.
- API: analytics and orchestration are exposed through API route handlers and surfaced to web clients through typed contracts.
- Frontend: notebook client panels render K3H4 availability, convergence, cluster counts, and ABI layer coverage state.

### Production Expectations

- Frontend route health panel should not show notebook manifest or notebook payload transport errors.
- Notebook manifest endpoint should resolve from API at `/api/notebooks/catalog-index.json`.
- Notebook payload endpoint should resolve from API at `/api/notebooks/native-c-catalog.ipynb`.
- Netcode analytics endpoint should resolve from API at `/api/netcode/analytics`.

### Quick Verification

Use these checks when production appears stale or partially deployed:

```bash
curl -i https://api.banana.engineer/health
curl -i https://api.banana.engineer/api/notebooks/catalog-index.json
curl -i https://api.banana.engineer/api/notebooks/native-c-catalog.ipynb
curl -i -X POST https://api.banana.engineer/api/netcode/analytics \
	-H "content-type: application/json" \
	--data '{"version":"k3h4-v1","k3h4Mode":"k3h4","spectralMode":"closed-form","sampleCount":4,"dims":4,"vectorSeed":42,"interactionSignal":0.55}'
```

If health is green but any contract endpoint is missing or returns 404, treat it as a deployment drift issue and re-run the main rollout workflow before validating frontend behavior.

<img width="1656" height="846" alt="image" src="https://github.com/user-attachments/assets/7f94328b-b466-4928-b377-61fd544ce5ff" />

<img width="2116" height="1061" alt="image" src="https://github.com/user-attachments/assets/8232a49b-fcf2-4366-974b-6b2e8b5207d8" />

<img width="823" height="1172" alt="image" src="https://github.com/user-attachments/assets/ae039daa-7b4e-443c-b8fb-2625058a110a" />
