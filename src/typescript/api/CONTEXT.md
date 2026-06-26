# API Service Context

## Purpose

Define backend service behavior, domain use cases, and service-side integration contracts.

## In scope

- Application/domain logic and route behavior
- Persistence and composition concerns
- Native integration calls from service code

## Out of scope

- Client rendering and UX behavior
- Native engine implementation internals

## Ubiquitous language

- Use case: Service-level operation that captures business intent
- Contract: Request/response or integration boundary guaranteed by the service

## Core invariants

- Public API contracts must be explicit and version-safe
- Domain rules should be enforceable independently from transport/framework concerns

## Key seams

- Native C ABI integration boundary
- Client-facing HTTP/schema contracts
- Shared TypeScript packages consumed by API and client

## Open questions

- None yet
