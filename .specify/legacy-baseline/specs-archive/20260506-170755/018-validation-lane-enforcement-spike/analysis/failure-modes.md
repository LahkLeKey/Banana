# 018 SPIKE -- Failure mode classification

Source: T003. Maps each HIGH-severity gap from
[drift-audit.md](./drift-audit.md) to a root cause.

## Failure modes

### FM-A -- `/speckit.tasks` template omission

The early task templates (pre-SPIKE-013) did not include a "Validation
phase mirrors spec.md Validation lane" gate. Authors who wrote 010,
011, 012 had no template prompt to add coverage tasks even though the
spec declared coverage thresholds.

**Affected gaps**: 010, 011, 012 coverage gaps (3).

### FM-B -- Author shorthand at close-out

Slice 017 used the post-013 template which DOES list the coverage
threshold script in spec.md, but the author scoped tasks.md to the
minimum viable set ("dotnet test green" instead of mirroring the full
Validation lane). Bulk-marking `[x]` via regex hid the omission.

**Affected gaps**: 017 coverage threshold (1).

### FM-C -- Cross-slice copy-paste decay

Not observed in this audit (would manifest as a slice copying an older
tasks.md skeleton wholesale). All gaps are explainable by FM-A or
FM-B.

**Affected gaps**: 0.

### FM-D -- Absence of a parity check

The meta cause shared by both FM-A and FM-B. There is no automated or
process step that re-reads `spec.md` Validation lane at close-out and
verifies each command appears in `tasks.md`. Without this check,
template improvements (FM-A fix) and author discipline (FM-B fix) both
degrade silently over time.

**Affected gaps**: ALL (4 of 4 HIGH-severity gaps).

## Conclusion

FM-D is the dominant cause. Fixing FM-A (template) prevents future
omissions of new validators but does not catch authors who still scope
tasks.md narrowly. Only an automated parity check (FM-D fix) catches
both classes deterministically. This points option A or option C from
[enforcement-options.md](./enforcement-options.md) over option B.
