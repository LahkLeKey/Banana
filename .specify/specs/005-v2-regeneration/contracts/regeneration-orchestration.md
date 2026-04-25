# Contract — Regeneration Orchestration

This contract governs how the v2 regeneration is sequenced and how slices
interact during cutover. It is referenced by every child spec.

## Roles

- **Parent spec** (`005-v2-regeneration`): owns the cross-slice dependency
  table, topological order, and global success criteria. Does not own code.
- **Child slice** (`006..014`): owns its own v2 target, hard contracts, plan,
  tasks, and cutover steps.
- **Legacy baseline** (`.specify/legacy-baseline/<domain>.md`): read-only
  factual record of v1. Never aspirational.

## Sequencing rules

1. Child slices may be **planned** in any order.
2. Child slices may be **implemented** in any order that respects the
   dependency edges declared in the parent `spec.md` table.
3. **Cutover** for slice X requires every slice that consumes X to have a
   working v2 against X's v2 entry points (or a documented compat shim).
4. Topological order of record:
   `006 → 008 → 007 → 009 → 010, 011 → 012 → 014 → 013`.

## Hard-contract preservation

- Each child spec MUST list a "Hard contracts to preserve" section.
- A v2 PR that changes a hard contract MUST update the child spec in the same PR.
- Hard contracts include (non-exhaustive): env var names, exit codes, route
  shapes, ABI symbols + numeric status codes, file paths used by automation,
  package names consumed across slices.

## Pain-point addressing

- Each child spec MUST identify which pain points from its baseline it
  addresses, defers (with rationale), or rejects (with rationale).
- A pain point listed in a baseline that is not addressed in the corresponding
  child spec is a review-blocking gap.

## Wiki & docs

- `.wiki/` remains frozen at the 12-file allowlist
  (`.specify/wiki/human-reference-allowlist.txt`).
- Per-slice human-facing docs go through `scripts/workflow-sync-wiki.sh` only
  if the allowlist is intentionally expanded; otherwise stay inside `.specify/`.
- AI-consumable docs live under `.specify/wiki/` and per-slice folders.

## Validation gate

The following must be green at the end of scaffolding and at every cutover:

```bash
python scripts/validate-ai-contracts.py
```

Per-slice gates live in each child `quickstart.md`.

## Out of scope

- Issue tracking (the user manually closed all v1 issues; planning is in `.specify/`).
- Refactoring legacy code outside a child slice's cutover step.
- Wiki expansion outside the allowlist process.
