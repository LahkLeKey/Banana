---
name: debug-banana-mobile-runtime
description: Debug Banana Android emulator and iOS preview runtime failures on Ubuntu WSL2.
argument-hint: Describe the failing mobile launcher command, emulator error, WSLg issue, or endpoint behavior.
agent: mobile-runtime-agent
---

Debug this Banana mobile runtime failure.

Requirements:

- Map the failure to Windows launcher, Ubuntu launcher, or endpoint readiness stage.
- Check WSL2/WSLg and Android SDK assumptions before assuming product regressions.
- Keep iOS simulator limitations explicit for Ubuntu workflows.
- If you fix the issue, validate the nearest launch path and summarize root cause.

Relevant assets:

- [mobile-runtime.instructions.md](../instructions/mobile-runtime.instructions.md)
- [banana-mobile-runtime](../skills/banana-mobile-runtime/SKILL.md)
- [banana-ci-debugging](../skills/banana-ci-debugging/SKILL.md)
## Wiki Updater Contract

- Treat this prompt as a wiki-synced contract surface: when prompt behavior, scope, assets, or acceptance criteria changes, keep the corresponding wiki snapshot current in the same SDLC run.
- Use `scripts/workflow-sync-wiki.sh` (or SDLC orchestration wrappers) so prompt updates and wiki docs are delivered together.
- If a prompt change introduces new automation flow, branch policy, validation step, or contract dependency, update prompt-linked wiki content before closing the change.

