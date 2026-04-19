# Build And Run Entry Points

## Native

- Workspace task: `Build Native Library`
- Script: `scripts/build-native.bat` on Windows
- Coverage build path: `scripts/run-native-c-tests-with-coverage.sh`
- Primary helpers: `native-core-agent`, `native-dal-agent`, `native-wrapper-agent`

## ASP.NET

- Workspace task: `Build Banana API`
- Full build script: `scripts/build-all.sh`
- Local run helper: `scripts/run-api.sh` when available in the current environment
- Primary helpers: `api-pipeline-agent`, `api-interop-agent`

## Frontend

- React dev: `bun run dev` in `src/typescript/react`
- React build: `bun run build` in `src/typescript/react`
- Electron smoke: `npm run smoke` in `src/typescript/electron`
- Primary helpers: `react-ui-agent`, `electron-agent`

## Compose And Runtime

- Full apps stack: `bash scripts/compose-apps.sh`
- Detached full apps stack: `DETACH=1 bash scripts/compose-apps.sh`
- Stop apps stack: `bash scripts/compose-apps-down.sh`
- Test profile: `bash scripts/compose-tests.sh`
- Runtime profile: `bash scripts/compose-runtime.sh`
- Primary helpers: `compose-runtime-agent`, `workflow-agent`

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

## Cross-Domain Teaming Protocol

- Follow [domain-teaming-playbook.md](../../agents/domain-teaming-playbook.md) for ownership boundaries, handoff packet expectations, and escalation rules.
- Identify the primary owner, supporting helpers, and validation owner before implementation.
- When handing work to another agent, include objective, ownership reason, touched files, contract impacts, validation state, and open risks.
- Preserve context continuity by carrying forward confirmed assumptions and prior validation signals.
- Escalate to `banana-sdlc` for multi-domain implementation orchestration and `integration-agent` for multi-domain validation orchestration.
