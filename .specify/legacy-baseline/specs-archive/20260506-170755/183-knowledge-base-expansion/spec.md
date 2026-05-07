# Feature Specification: Knowledge Base Expansion (183)

**Feature Branch**: `183-knowledge-base-expansion`
**Created**: 2026-05-03
**Status**: Draft
**Input**: Knowledge page has inert Spaces, only three static GitHub links, no search/filter, and no live health context. Needs meaningful developer experience.

## Problem Statement

The Knowledge page is a skeleton: Spaces are decorative, articles are three static links that open GitHub, there is no discovery or filtering, and the runtime health/model status context that belongs here is absent.

## In Scope

- E1: Article search/filter by keyword across all articles.
- E2: Spaces as live category filters that narrow the article list.
- E3: Richer article corpus — expand from 3 to ~12 categorised entries covering all major Banana domains.
- E4: Live system health strip from /health showing API, model, and runtime status.
- E5: Article inline preview (expand a summary inline instead of always opening GitHub).

## Out of Scope

- Full-text search over external GitHub files.
- CMS or database-backed article management.
- Authentication-gated content.

## User Scenarios & Testing

### User Story 1 - Find Articles Quickly (Priority: P1)

As a developer, I can search and filter the knowledge base by keyword and space category.

**Independent Test**: Type keyword, verify only matching articles appear. Click a space, verify articles filter to that category.

### User Story 2 - See Platform Health in Context (Priority: P1)

As an operator, I can see live system health on the Knowledge page without navigating away.

**Independent Test**: Load page, verify health strip shows API status from /health endpoint.

### User Story 3 - Browse Richer Article Set (Priority: P2)

As a developer, I can find articles across all Banana domains, not just three entries.

**Independent Test**: Article list shows entries across at least four spaces/categories.

## Requirements

- FR-001: Search input MUST filter articles by title and summary keyword in real-time.
- FR-002: Clicking a Space MUST filter articles to that category (All resets).
- FR-003: Article corpus MUST cover all four Spaces with at least two articles each.
- FR-004: Health strip MUST show live API /health status with graceful fallback.
- FR-005: Each article MUST have an inline expand/collapse preview showing summary in-place.

## Success Criteria

- SC-001: Keyword search returns matching subset within one keystroke.
- SC-002: Space filter narrows article list correctly.
- SC-003: Health strip shows live status or "unavailable" fallback.
