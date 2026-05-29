# 031 Daily PR Checklist

Use this checklist for any coherent-world PR.

## 1) Guardrail

- Run:
  - `bash scripts/validate-coherent-world-unified-slice.sh`
- Expected:
  - pass with no superseded/new coherent-lane slice violations.

## 2) Native Evidence Refresh

- Run:
  - `bash scripts/refresh-coherent-world-evidence.sh`
- Artifacts refreshed:
  - `artifacts/native/031-unified-coherent-world/umbrella-baseline.txt`
  - `artifacts/native/031-unified-coherent-world/full-engine-baseline.txt`
  - `artifacts/native/031-unified-coherent-world/full-mmo-demo.txt`

## 3) API Evidence Refresh

- Run:
  - `bash scripts/refresh-coherent-world-api-baselines.sh`
- Artifacts refreshed:
  - `artifacts/typescript/api/coherent-world-units.log`
  - `artifacts/typescript/api/coherent-world-integration.log`

## 4) Spec + Dashboard Sync

- Confirm `031` baselines in spec match refreshed evidence.
- Confirm `execution-dashboard.md` row for 031 remains accurate.

## 5) PR Ready Signal

- Guard passed.
- Native and API evidence files updated.
- 031 docs reflect current run results.
