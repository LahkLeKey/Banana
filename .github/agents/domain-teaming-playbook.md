# Domain Teaming Playbook

Use this playbook when coordinating Banana work across agents so specialists operate as one team with shared context.

## Domain Responsibility Map

- Native domain: `native-core-agent`, `native-dal-agent`, `native-wrapper-agent`, coordinated by `native-c-agent`
- ASP.NET domain: `api-pipeline-agent`, `api-interop-agent`, coordinated by `csharp-api-agent`
- Frontend domain: `react-ui-agent`, `electron-agent`, coordinated by `react-agent`
- Runtime and CI domain: `compose-runtime-agent`, `workflow-agent`, coordinated by `infrastructure-agent`
- Cross-domain quality and orchestration: `integration-agent`, `test-triage-agent`, `banana-reviewer`, `banana-planner`, `banana-sdlc`

## Team-First Operating Model

1. Discover touched files, runtime contracts, and validation scope before editing.
2. Select the narrowest owner agent for each slice.
3. Handoff immediately when a slice crosses ownership boundaries.
4. Keep context moving with each handoff instead of restarting discovery.
5. Validate on the narrowest surface first, then expand only when contracts cross domains.

## Required Handoff Packet

Every handoff should include:

1. Outcome goal and expected behavior.
2. Why this target agent owns the next slice.
3. Touched files and likely follow-on files.
4. Contract impacts, especially:
   - `BANANA_PG_CONNECTION`
   - `BANANA_NATIVE_PATH`
   - `VITE_BANANA_API_BASE_URL`
   - `@banana/ui` shared package and Tailwind preset integration
5. Validation already run, validation still required, and failure signals to watch.
6. Open assumptions, risks, and rollback notes.

## Inbound Handoff Checklist

Before coding on a received handoff:

1. Restate the goal and owning scope.
2. Confirm contract assumptions.
3. Confirm validation expectations.
4. Execute the slice or reroute to a narrower owner with updated packet details.

## Escalation Rules

- Escalate to `banana-sdlc` when more than two domains must move together.
- Escalate to `integration-agent` when validation spans multiple domains.
- Escalate to `banana-reviewer` when residual risk remains after implementation.
