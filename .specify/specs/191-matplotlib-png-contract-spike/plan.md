# Implementation Plan: Matplotlib PNG Contract Spike (191)

**Branch**: `191-matplotlib-png-contract-spike` | **Date**: 2026-05-03 | **Spec**: spec.md
**Input**: Feature specification from `.specify/specs/191-matplotlib-png-contract-spike/spec.md`

## Summary

Investigate the Matplotlib PNG output path specifically for the Banana Data Science page. The goal is to define how static scientific charts should be emitted, rendered, saved, and exported without committing the page to heavier interactive tooling.

## Technical Context

**Language/Version**: TypeScript React app, browser-executed Python notebooks
**Primary Dependencies**: Pyodide runtime, Data Science page notebook renderer, Matplotlib output conventions
**Storage**: Local browser notebook state and exported notebook JSON
**Testing**: Spike documentation review plus spec/tasks parity validation
**Target Platform**: Banana Data Science page in the browser
**Project Type**: Frontend notebook visualization investigation
**Performance Goals**: Keep static scientific charts readable and predictable without heavy runtime overhead
**Constraints**: Must fit current notebook import/export flow and page-level rendering constraints
**Scale/Scope**: One narrow DS-page visualization contract spike

## Constitution Check

Spike only. No constitution exceptions expected.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/191-matplotlib-png-contract-spike/
├── analysis/
├── plan.md
├── spec.md
└── tasks.md
```

### Source Code (repository root)

```text
src/typescript/react/src/pages/
src/typescript/react/public/notebooks/
```

**Structure Decision**: This spike is scoped to Banana's Data Science page contract and should produce analysis artifacts plus a bounded follow-up readiness packet.