# Client Applications Context

## Purpose

Define client-facing behavior, interaction flows, and integration with backend/runtime surfaces.

## In scope

- UI behavior and interaction patterns
- Client-side state and shell/runtime integration
- Consumption of API and shared package contracts

## Out of scope

- Server-side domain/persistence internals
- Native engine algorithm internals

## Ubiquitous language

- Flow: User-visible sequence of interactions and state transitions
- Client contract: Assumption the UI makes about API/shared package behavior

## Core invariants

- User-visible behavior should be consistent across supported runtime modes
- Client assumptions about API contracts must remain explicit and testable

## Key seams

- API service request/response boundaries
- Shared UI and resilience package boundaries

## Open questions

- None yet
