# Shared Contracts and Libraries Context

## Purpose

Define cross-context reusable contracts and package APIs shared by API and client code.

## In scope

- Shared types and reusable library APIs
- Cross-context compatibility and versioning expectations
- Shared UI/resilience primitives used by multiple applications

## Out of scope

- App-specific feature logic owned by a single context

## Ubiquitous language

- Shared contract: Reusable, stable type or API used across contexts
- Compatibility: Ability for API and client consumers to upgrade safely

## Core invariants

- Shared contract changes must account for all known consumers
- Breaking changes require explicit versioning/migration strategy

## Key seams

- API service consumers
- Client application consumers
- Native integration type adapters where applicable

## Open questions

- None yet
