# Banana

Banana is a multi-language monorepo with native runtime systems, TypeScript APIs, React surfaces, and delivery automation.

https://banana.engineer/

<img width="2557" height="1270" alt="image" src="https://github.com/user-attachments/assets/7d60c581-75d4-4137-a03f-4888be440f94" />

## Domains

- Native engine and runtime: `src/native`
- TypeScript API and web surfaces: `src/typescript`
- Delivery scripts and orchestration: `scripts`, `docker`, `.github/workflows`

## Public Packages

The following packages are currently published for external consumption.

| Package | Latest | Install | Demo |
| --- | --- | --- | --- |
| `banana-panels` | `0.1.2` | `bun add banana-panels` or `npm install banana-panels` | https://panels.banana.engineer/ |

## Package Notes

- `banana-panels` is the public panel-system package currently used for iterative external adoption.
- Internal scoped packages under `@banana/*` remain workspace-private unless explicitly listed above.

## Verification Shortcuts

- API smoke: `API: smoke (v3)` task
- Shared UI smoke: `UI: smoke (v3)` task
- Panels package build: `cd src/typescript/packages/panels && bun run build`

## Agent Tooling

Use these files and commands for a fast return-to-work flow.

- Agent entrypoint and conventions: `AGENTS.md`
- Domain routing map: `CONTEXT-MAP.md`
- Return checklist: `docs/agents/return-to-work-checklist.md`
- Issue tracker rules: `docs/agents/issue-tracker.md`
- Triage vocabulary: `docs/agents/triage-labels.md`
- Domain doc rules: `docs/agents/domain.md`

Kickoff commands from repo root:

- Readiness snapshot: `npm run kickoff:status`
- Full kickoff (API + UI smoke): `npm run kickoff:return`
- Auto-fix deps + kickoff: `npm run kickoff:return:auto-fix`

Issue template:

- `.github/ISSUE_TEMPLATE/implementation-intake.md`
