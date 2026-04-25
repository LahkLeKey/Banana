---
name: implement-banana-mobile-runtime-change
description: Implement a focused Banana mobile runtime change for Ubuntu WSL2 emulator channels.
argument-hint: Describe the mobile launcher, Android emulator, iOS preview fallback, or WSL2 runtime behavior to change.
agent: mobile-runtime-agent
---

Implement this Banana mobile runtime change with focused ownership.

Requirements:

- Keep work centered on mobile runtime scripts and WSL2 launcher contracts.
- Reuse existing compose bring-up entry points before adding runtime automation.
- Preserve Ubuntu-first distro selection and fail-fast preflight behavior.
- Keep iOS simulator messaging explicit for Ubuntu (web-preview fallback only).
- Summarize runtime changes, validation run, assumptions, and residual risks.

Relevant assets:

- [mobile-runtime.instructions.md](../instructions/mobile-runtime.instructions.md)
- [infra.instructions.md](../instructions/infra.instructions.md)
- [banana-mobile-runtime](../skills/banana-mobile-runtime/SKILL.md)

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.
## Wiki Updater Contract

- Treat this prompt as a wiki-synced contract surface: when prompt behavior, scope, assets, or acceptance criteria changes, keep the corresponding wiki snapshot current in the same SDLC run.
- Use `scripts/workflow-sync-wiki.sh` (or SDLC orchestration wrappers) so prompt updates and wiki docs are delivered together.
- If a prompt change introduces new automation flow, branch policy, validation step, or contract dependency, update prompt-linked wiki content before closing the change.

