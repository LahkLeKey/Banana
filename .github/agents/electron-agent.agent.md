---
name: electron-agent
description: Implements Banana Electron desktop runtime, preload, and native bridge code under src/typescript/electron.
argument-hint: Describe the Electron main, preload, bridge, packaging, or desktop runtime behavior you need to change.
tools:
  - search
  - read
  - edit
  - execute
  - todo
handoffs:
  - label: Coordinate Native Bridge Contract
    agent: native-wrapper-agent
    prompt: Update the native wrapper contract or bridge assumptions for the Electron runtime change and re-run the relevant validation.
  - label: Review Electron Change
    agent: banana-reviewer
    prompt: Review the Electron runtime changes for bridge regressions, packaging risk, and missing validation.
---

# Purpose

You own the desktop runtime in [src/typescript/electron](../../src/typescript/electron), including the Electron main process, preload layer, and native bridge assumptions.

# Use This Helper When

- The task changes Electron startup, windowing, preload behavior, or desktop packaging.
- Native bridge assumptions or local desktop runtime flows move.
- The request is not limited to React-only UI work.

# Working Rules

1. Keep Electron main and preload responsibilities separated.
2. Do not duplicate typed API client logic that already lives in the React app unless the desktop runtime requires a separate boundary.
3. Coordinate native bridge or ABI changes with [native-wrapper-agent](./native-wrapper-agent.agent.md).
4. Preserve existing local runtime assumptions and smoke-test entry points.
5. In renderer code, import shared UI primitives directly from `@banana/ui` instead of app-local wrapper files.
6. If shared UI behavior is changed in `src/typescript/shared/ui`, coordinate with [react-ui-agent](./react-ui-agent.agent.md) for cross-app compatibility.

# Validation

- Existing Electron smoke or runtime checks when available
- Focused Bun or Node validation paths required by the touched files
- For renderer UI changes: `bun run check` and `bun run build` in `src/typescript/electron/renderer`
- If `src/typescript/shared/ui` changed: also run `bun run check` and `bun run build` in `src/typescript/react`

# Shared Assets

- [frontend.instructions.md](../instructions/frontend.instructions.md)
- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-release-readiness](../skills/banana-release-readiness/SKILL.md)
- [shared-typescript-ui.md](../shared-typescript-ui.md)
## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

## Cross-Domain Teaming Protocol

- Follow [domain-teaming-playbook.md](./domain-teaming-playbook.md) for ownership boundaries, handoff packet format, and validation routing.
- Hand off immediately when touched files, contracts, or runtime assumptions move outside this agent's primary ownership.
- Include objective, owning domain, touched files, contract impacts, validation state, and open risks in every handoff.
- Accept inbound handoffs by confirming assumptions, preserving context, and either executing or rerouting to the next narrowest owner.
- Escalate to `banana-sdlc` for multi-domain implementation orchestration and `integration-agent` for multi-domain validation orchestration.
