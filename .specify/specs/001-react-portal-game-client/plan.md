# Implementation Plan: React Portal Game Client Pivot

## Goal

Deliver a slim, maintainable monorepo control surface centered on a game-client React portal while preserving live functionality.

## Scope Plan

1. Keep one active pivot spec in `.specify/specs`.
2. Keep minimal workflows in `.github/workflows`:
- React CI
- React deploy
3. Continue incremental React UX reflow toward viewport-first game-client behavior.

## Validation Strategy

- Local: `bun run build` in `src/typescript/react` with `VITE_BANANA_API_BASE_URL` set.
- CI: workflow install + build contract validation.
- Runtime: smoke navigation across core portal routes.

## Risks

- Missing deploy secrets can block automated release.
- Reintroduced legacy files can expand scope accidentally.

## Mitigations

- Make deploy workflow manual + push-main and clearly secret-gated.
- Keep active roots intentionally tiny and documented.
