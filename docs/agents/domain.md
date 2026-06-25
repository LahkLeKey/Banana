# Domain Docs

How engineering skills should consume this repo's domain documentation.

## Before exploring, read these

- CONTEXT-MAP.md at the repo root if it exists; use it to find the relevant per-context CONTEXT.md files
- CONTEXT.md files for only the contexts relevant to the task
- docs/adr/ for system-wide ADRs
- src/<context>/docs/adr/ for context-scoped ADRs

If any are missing, proceed silently. Do not block or require creating docs upfront.

## File structure

This repo uses a multi-context layout:

/
- CONTEXT-MAP.md
- docs/adr/ (system-wide ADRs)
- src/<context>/CONTEXT.md
- src/<context>/docs/adr/

## Use glossary vocabulary

When naming domain concepts in outputs, use terms from the relevant CONTEXT.md.

## Flag ADR conflicts

If a proposal conflicts with an ADR, call out the conflict explicitly.
