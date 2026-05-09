# Feature Specification: Wiki Rescaffold Baseline

**Feature Branch**: `004-wiki-rescaffold-baseline`
**Created**: 2026-05-09
**Status**: Draft
**Input**: Destroy current research-era wiki surface and rebuild a clean, scoped wiki baseline aligned to current product and runtime direction.

## In Scope

- Remove current active wiki content that reflects out-of-scope research-era direction.
- Re-scaffold `.specify/wiki/human-reference/` with a minimal approved information architecture.
- Re-scaffold `.wiki/` publication surface to match the new canonical source.
- Rebuild allowlist and sync touchpoints so only approved pages can be published.

## Out of Scope

- Expanding wiki coverage beyond approved baseline sections in this slice.
- Rewriting archived wiki/history artifacts under `.specify/legacy-baseline/`.
- Product feature implementation work outside wiki and documentation infrastructure.

## User Scenarios & Testing

### User Story 1 - Clean Wiki Baseline (Priority: P1)

As a maintainer, I can inspect wiki roots and find only in-scope baseline pages.

**Why this priority**: Eliminates immediate documentation drift and scope confusion.

**Independent Test**: List pages under `.specify/wiki/human-reference/` and `.wiki/` and verify only approved baseline pages exist.

**Acceptance Scenarios**:

1. **Given** the rescaffold is complete, **When** I enumerate wiki files, **Then** only baseline-approved pages are present.

---

### User Story 2 - Source-Publish Alignment (Priority: P1)

As an automation owner, I can run wiki sync and get deterministic output because source and publication trees share the same approved structure.

**Why this priority**: Prevents reintroduction of stale research pages during sync.

**Independent Test**: Run wiki sync in dry-run mode and verify no unexpected files are proposed.

**Acceptance Scenarios**:

1. **Given** `.specify/wiki/human-reference/` as source of truth, **When** sync runs, **Then** `.wiki/` mirrors only approved pages.

---

### User Story 3 - Governance Enforced (Priority: P2)

As QA, I can rely on allowlist enforcement to block unreviewed wiki growth.

**Why this priority**: Keeps baseline stable after cleanup.

**Independent Test**: Attempt to add an out-of-allowlist page and verify validation fails.

**Acceptance Scenarios**:

1. **Given** an unapproved page path, **When** validation runs, **Then** it is rejected with a clear governance message.

### Edge Cases

- Sync scripts still reference deleted research-era pages.
- Home/index links point to removed paths.
- Allowlist permits stale directories by wildcard drift.

## Requirements

### Functional Requirements

- **FR-001**: Existing research-era wiki pages in active roots MUST be removed or replaced by baseline-approved pages.
- **FR-002**: `.specify/wiki/human-reference/` MUST define the canonical wiki source tree for approved baseline sections only.
- **FR-003**: `.wiki/` MUST be regenerated to match canonical source and remove stale pages.
- **FR-004**: `.specify/wiki/human-reference-allowlist.txt` MUST include only approved baseline paths.
- **FR-005**: Wiki sync/validation documentation MUST describe the new baseline and publication process.

### Key Entities

- **CanonicalWikiSource**: `.specify/wiki/human-reference/` content approved for AI-facing canonical documentation.
- **PublishedWikiSurface**: `.wiki/` pages generated or synchronized from canonical source.
- **WikiAllowlistPolicy**: Path-level guardrails that constrain allowed wiki content growth.

## Success Criteria

### Measurable Outcomes

- **SC-001**: Wiki file count in active roots decreases to the approved baseline set with zero stale research pages.
- **SC-002**: Sync dry-run reports zero unexpected page creations outside allowlist.
- **SC-003**: Validation fails deterministically for out-of-allowlist wiki paths.
- **SC-004**: All baseline wiki index/home links resolve to existing pages.

## Assumptions

- `.specify/wiki/human-reference/` remains the canonical source and `.wiki/` remains publication output.
- Team agrees to rebuild baseline content incrementally after the reset.
- Archived historical documentation remains available in non-active archival locations.
