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