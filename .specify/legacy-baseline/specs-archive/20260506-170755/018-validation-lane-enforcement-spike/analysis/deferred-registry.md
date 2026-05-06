# 018 SPIKE -- Deferred registry

Source: T007. Items intentionally pushed past slice 019.

## D-001 -- Finalize-feature hook (option C)

- **Source**: [enforcement-options.md](./enforcement-options.md) option C.
- **Disposition**: `defer-with-trigger`.
- **Trigger condition**: Slice 019's CI lint demonstrates author
  bypass attempts (e.g. authors marking tasks `[x]` and merging
  without the lint passing locally because they pushed directly to a
  branch that skipped CI). If observed twice, escalate to option C as
  a local-mandatory hook.
- **Owner when triggered**: workflow-agent.

## D-002 -- Coverage evidence file convention

- **Source**: New idea raised during option A scoring. Each slice
  could emit `analysis/validation-evidence.md` containing the actual
  command outputs.
- **Disposition**: `defer-with-trigger`.
- **Trigger condition**: Slice 019 ships and we observe a case where
  the lint passes but the validator was not actually run (e.g. a
  task marked `[x]` without being executed). Then add the evidence
  file requirement to the lint.
- **Owner when triggered**: technical-writer-agent + workflow-agent.

## D-003 -- Pre-013 slice coverage backfill (010, 011, 012)

- **Source**: [drift-audit.md](./drift-audit.md) HIGH gaps.
- **Disposition**: `ready-now` -- folded into slice 019 acceptance
  bar A3. Author decision per slice (add coverage task vs. remove
  threshold from spec).
- **NOT deferred**; listed here for traceability only.
