# Banana

Pass the Banana from native C into API, desktop, web, and mobile channels.

This repo is a practical playground for one core idea: keep domain behavior in native C, then project it cleanly into multiple runtimes without rewriting business logic per app.

| Payload (text)                                             | banana_score | label       |
|------------------------------------------------------------|--------------|-------------|
| `ripe banana peel smoothie banana bunch banana bread`      | ~0.9507      | banana      |
| `banana banana banana banana banana banana banana banana`  | ~0.9441      | banana      |
| `yellow fruit on the counter maybe`                        | ~0.8287      | banana      |
| `""` (empty)                                               | ~0.6454      | banana      |
| `yellow plastic toy shaped like a banana` (decoy)          | ~0.4404      | not_banana  |
| `plastic engine oil junk waste motor oil`                  | ~0.0782      | not_banana  |


## Learn More

- Wiki hub: [Banana Wiki](https://github.com/LahkLeKey/Banana/wiki)
- Onboarding and architecture: `docs/developer-onboarding.md`
- Build/test command index: `.wiki/Build-Run-Test-Commands.md`

> Banana is a prototype for Poly: a polymorphic, multi-platform runtime approach where core behavior starts in C and can be projected into API, desktop, frontend, and mobile delivery channels.

## Coverage 80 One-Pass Triage Checklist

Use this checklist when a merge-gated coverage run fails:

1. Open the coverage aggregate artifact and identify failing tuples first.
2. Inspect each lane summary for status, failure class, measured percent, threshold percent, and deficit percent.
3. Confirm denominator policy snapshot version matches all evaluated lanes.
4. Confirm evidence bundle paths are workspace-relative and present in artifacts.
5. Verify whether active exceptions are listed and whether any exception has expired.
6. For integration tuple failures, confirm `BANANA_PG_CONNECTION` and `BANANA_NATIVE_PATH` contracts were satisfied.

## Spec Kit Workflow Wrapper

Use the parser-driven helper to run and inspect Spec Kit workflow state from one place:

```bash
bash scripts/workflow-specify.sh doctor
```

Useful commands:

```bash
bash scripts/workflow-specify.sh status
bash scripts/workflow-specify.sh paths
bash scripts/workflow-specify.sh run-full "Realign scope for native test coverage"
bash scripts/workflow-specify.sh run-workflow "Add API traceability gate" speckit copilot
```

Notes:

- The script auto-adds `C:/Users/Zephr/.local/bin` to `PATH` when available so `specify` can be discovered in Git Bash after `uv tool install`.
- `run-full` defaults to workflow `drift-realignment` and integration `copilot`.
- Override defaults with `BANANA_SPECIFY_WORKFLOW_ID` and `BANANA_SPECIFY_INTEGRATION`.
