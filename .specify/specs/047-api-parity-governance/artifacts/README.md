# API Parity Evidence Artifacts

This folder stores evidence produced by the 047 API parity governance validator.

## Files

- `overlapping-routes.inventory.json`: Normalized overlap inventory (`METHOD + path`) between ASP.NET and Fastify surfaces.
- `parity-exceptions.json`: Approved temporary parity exceptions with owner/rationale/expiry.
- `parity-drift-report.json`: Validator findings, including `missing_route` gaps from route-surface comparison.

## Regeneration

From repository root:

```bash
bash scripts/validate-api-parity-governance.sh --inventory-only
```

Or strict gap reporting mode:

```bash
bash scripts/validate-api-parity-governance.sh
```
