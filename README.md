# Banana

Banana is a multi-language monorepo with native runtime systems, TypeScript APIs, React surfaces, and delivery automation.

https://banana.engineer/

## Domains

- Native engine and runtime: `src/native`
- TypeScript API and web surfaces: `src/typescript`
- Delivery scripts and orchestration: `scripts`, `docker`, `.github/workflows`

## Public Packages

The following packages are currently published for external consumption.

| Package | Latest | Install | Demo |
| --- | --- | --- | --- |
| `banana-panels` | `0.1.0` | `bun add banana-panels` or `npm install banana-panels` | https://panels.banana.engineer/ |

## Package Notes

- `banana-panels` is the public panel-system package currently used for iterative external adoption.
- Internal scoped packages under `@banana/*` remain workspace-private unless explicitly listed above.

## Verification Shortcuts

- API smoke: `API: smoke (v3)` task
- Shared UI smoke: `UI: smoke (v3)` task
- Panels package build: `cd src/typescript/packages/panels && bun run build`
