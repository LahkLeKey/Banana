# Implementation Plan: Knowledge Base Expansion (183)

**Branch**: `183-knowledge-base-expansion` | **Date**: 2026-05-03 | **Spec**: [spec.md](spec.md)

## Overview

Expand Knowledge page from skeleton to a useful developer hub: keyword search, space-based category filtering, richer article corpus, live health strip, and inline article preview.

## Technical Context

**Language/Version**: TypeScript, React
**Primary Dependencies**: React app under `src/typescript/react`; existing `/health` API endpoint
**Storage**: Static article corpus in component; live health from API fetch
**Testing**: Type-check, browser behavior checks
**Target Platform**: banana.engineer
**Constraints**: Preserve existing layout grid; no new dependencies needed

## Constitution Check

- Domain layering preserved: PASS.
- Spec-first workflow preserved: PASS.
- Validation path defined: PASS.

## Workstreams

1. Expand article corpus with categorised entries for all four Spaces.
2. Add keyword search state + real-time filter.
3. Make Spaces sidebar filter articles by category.
4. Add health strip that fetches /health and shows live status.
5. Add inline expand/collapse per article card.

## Validation Approach

1. Type-check: `bun tsc --noEmit`
2. Browser: search narrows list, space click filters, health strip shows status or fallback, article expands inline.
