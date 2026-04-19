# Release Readiness Checklist

## Code And Contract

- The changed area matches the intended domain scope
- The narrowest practical helper agent owned each implementation slice
- Cross-layer contracts were preserved or explicitly documented
- Native interop changes were paired with managed validation when needed

## Validation

- Relevant build task or script was run
- Relevant focused tests were run
- Broader integration, compose, or coverage checks were run when risk justified them

## Documentation And Ops

- User-facing or developer-facing workflow changes are documented near the changed entry point
- Prompt, skill, or instruction references remain accurate after the change
- Runtime env vars and health check expectations still match the compose and workflow configuration

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
