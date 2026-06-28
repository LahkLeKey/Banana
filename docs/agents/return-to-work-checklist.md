# Return-to-Work Checklist

Use this checklist when you come back after time away.

Single-command kickoff:

- Readiness snapshot only: npm run kickoff:status
- kickoff:status reports git clean/dirty state and readiness
- From repo root: npm run kickoff:return
- Optional auto-fix mode: npm run kickoff:return:auto-fix

Status summary meanings:

- ALL GREEN: Git is clean and prerequisites are ready.
- READY WITH LOCAL CHANGES: Prerequisites are ready, but your working tree has local edits.
- ATTENTION NEEDED: One or more prerequisites are missing; follow the printed fix step.
- The kickoff command now runs a preflight first:
   - checks bun is available
   - checks TypeScript workspace dependencies exist
   - prints one fix command if something is missing

## Goal 1: Understand where things live fast

1. Read CONTEXT-MAP.md to pick the right context.
2. Read the matching context file for your area:
   - src/native/CONTEXT.md
   - src/native/engine/CONTEXT.md
   - src/typescript/api/CONTEXT.md
   - src/typescript/react/CONTEXT.md
   - src/typescript/shared/CONTEXT.md
3. Read ADRs in this order:
   - context ADR folder first
   - docs/adr second (global decisions)

## Goal 2: Run checks and tests without guessing

Default first health check after a break:

1. Run npm run kickoff:return (this runs API smoke, then UI smoke)
2. If both pass, continue with normal work
3. If needed, continue with native tasks

If API: smoke (v3) fails, default action:

1. Stop and do not continue to other tasks yet
2. Capture these 4 details:
   - Task name
   - First error line
   - Last error line
   - What changed just before failure
3. Open or update a GitHub issue with those details
4. Apply one intake label (usually needs-triage)

Use VS Code tasks first (they are the supported defaults):

- Native: configure (v3)
- Native: build (v3)
- Native: netcode suite (v3)
- Native: war suite (focused)
- API: smoke (v3)
- UI: smoke (v3)

If a task fails, capture:

1. Task name
2. First error line
3. Last error line
4. What changed just before failure

## Goal 3: Triage and create work items consistently

1. Read docs/agents/issue-tracker.md.
2. Read docs/agents/triage-labels.md.
3. For new work, create a GitHub issue with:
   - clear title
   - expected behavior
   - actual behavior
   - reproduction steps
   - scope (which context)
4. Apply one triage label at intake:
   - needs-triage
   - needs-info
   - ready-for-agent
   - ready-for-human
   - wontfix

## Quick start (10 minutes)

1. Run npm run kickoff:return (2 min)
2. Read CONTEXT-MAP.md (2 min)
3. Read one CONTEXT.md for your area (3 min)
4. If blocked, open/update an issue using triage labels (3 min)

## Notes for agent sessions

- Keep language simple and junior-friendly.
- Explain why a step matters before giving commands.
- Prefer small, reversible changes.
