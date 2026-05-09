# Implementation Plan: Wiki Rescaffold Baseline

**Branch**: `004-wiki-rescaffold-baseline` | **Date**: 2026-05-09 | **Spec**: `.specify/specs/004-wiki-rescaffold-baseline/spec.md`
**Input**: Feature specification from `.specify/specs/004-wiki-rescaffold-baseline/spec.md`

## Summary

Reset active wiki content to a minimal, in-scope baseline and re-establish deterministic source-to-publication synchronization with strict allowlist governance.

## Technical Context

**Language/Version**: Markdown, Bash, JSON
**Primary Dependencies**: wiki sync scripts, allowlist policy file, repository docs structure
**Storage**: File-based wiki content under `.specify/wiki` and `.wiki`
**Testing**: file inventory checks, link checks, sync dry-run validation
**Target Platform**: Local Git Bash and CI Linux runners
**Project Type**: Documentation governance and scaffolding refactor
**Performance Goals**: Fast deterministic wiki sync with no unbounded content growth
**Constraints**: Keep canonical/source model intact; avoid archived baseline rewrites
**Scale/Scope**: `.specify/wiki`, `.wiki`, scripts/docs that define wiki sync policy

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- Preserve canonical AI-consumable source in `.specify/wiki/`.
- Keep `.wiki/` as publication-only surface.
- Enforce allowlist-managed page growth.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/004-wiki-rescaffold-baseline/
├── plan.md
├── spec.md
└── tasks.md
```

### Source Code (repository root)

```text
.specify/wiki/
├── human-reference/
└── human-reference-allowlist.txt

.wiki/
scripts/
docs/
```

**Structure Decision**: Rebuild both canonical and publication wiki roots from approved baseline IA, then lock with allowlist governance.

## Complexity Tracking

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| None anticipated | N/A | N/A |
