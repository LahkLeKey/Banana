# Skills Inventory (v1 snapshot)

Source: `.github/skills/<name>/SKILL.md`. 16 skills. Verbatim content
preserved at `../github-ai-surface/skills/`.

## Banana skills (7)

| Skill | Scope |
| --- | --- |
| `banana-discovery` | First-load architecture + runtime contract discovery before planning. |
| `banana-agent-decomposition` | Break work into narrowest helper agents + handoffs. |
| `banana-build-and-run` | Pick correct build/run/compose entry point per touched domain. |
| `banana-test-and-coverage` | Unit/integration/e2e/native + coverage with smallest scope. |
| `banana-ci-debugging` | Triage workflow/compose/runtime failures to exact stage. |
| `banana-mobile-runtime` | Android emulator + iOS web-preview launch on WSL2/WSLg. |
| `banana-release-readiness` | Final review across code/tests/docs/runtime/automation. |

## Spec Kit skills (9)

`speckit-analyze`, `speckit-checklist`, `speckit-clarify`,
`speckit-constitution`, `speckit-implement`, `speckit-plan`,
`speckit-specify`, `speckit-tasks`, `speckit-taskstoissues`.

## v2 Constraints

- Skills are the documented entry points for AI execution flows; the spec
  catalog lives in spec 013 invariants.
- `banana-discovery` MUST stay the first-loaded skill — it teaches readers
  where the runtime contracts and helper-agent map live.
