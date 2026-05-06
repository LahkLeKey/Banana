# Upstream Impact Matrix Template

## Candidate

- Candidate ID:
- Candidate title:

## Impacted Domains

- native
- aspnet
- typescript-api
- react/ui
- tests/automation

## Contract Surfaces Potentially Affected

- status mapping / error contract
- payload shape contract
- route exposure contract
- runtime env contract (`BANANA_NATIVE_PATH`, `BANANA_PG_CONNECTION`, `VITE_BANANA_API_BASE_URL`)

## Validation Lanes Required for Follow-Up

- `tests/native`
- `tests/unit`
- `tests/e2e`
- `src/typescript/api` test suite

## Residual Risks

- risk 1:
- risk 2:

## Sequencing Notes

- must precede:
- can run in parallel with:
