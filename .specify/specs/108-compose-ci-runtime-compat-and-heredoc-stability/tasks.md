# Tasks: 108-compose-ci-runtime-compat-and-heredoc-stability

## T001 - Restore runtime compatibility exceptions registry path

**Status**: Planned
**File(s)**: `.github/workflows/runtime-compatibility-exceptions.yml`, `.github/workflows/compose-ci.yml`
**Acceptance**: Compose CI runtime compatibility lane no longer reports `exceptions_registry_present=false`.

---

## T002 - Repair .NET integration fallback artifact snippet syntax

**Status**: Planned
**File(s)**: `.github/workflows/compose-ci.yml`
**Acceptance**: no shell warnings/errors about heredoc termination in integration fallback step.

---

## T003 - Repair .NET unit fallback artifact snippet syntax

**Status**: Planned
**File(s)**: `.github/workflows/compose-ci.yml`
**Acceptance**: no shell warnings/errors about heredoc termination in unit fallback step.

---

## T004 - Add syntax safety guard for multiline fallback shell logic

**Status**: Planned
**File(s)**: workflow/script validation surface
**Acceptance**: malformed multiline shell blocks fail pre-merge validation.

---

## T005 - Capture Compose CI closure evidence

**Status**: Planned
**File(s)**: `.specify/specs/108-compose-ci-runtime-compat-and-heredoc-stability/tasks.md`
**Acceptance**: includes run IDs and lane outcomes for runtime compatibility + dotnet coverage lanes.
