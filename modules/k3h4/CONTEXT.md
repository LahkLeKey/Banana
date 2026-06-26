# K3H4 Core Module Context

## Purpose

Define the K3H4 native core as a single module boundary with one public library surface.

## In scope

- K3H4 native source moved from legacy native engine locations
- K3H4 native tests moved with the module
- Public K3H4 native interface consumed by runtime and API integration layers

## Out of scope

- API route and client feature ownership for K3H4
- Gameplay-focused suites outside K3H4 core validation

## Ubiquitous language

- K3H4 core module: The isolated native K3H4 implementation at modules/k3h4/native
- Public module boundary: The only supported include and link surface for external consumers
- One-shot migration: A single change that moves K3H4 core code and rewires all call sites

## Core invariants

- Consumers link through one target: banana_k3h4_core
- Consumers use only module-prefixed public includes
- Module internals are not directly included by external code

## Key seams

- Native engine runtime consumes K3H4 through the public module boundary
- API native adapters consume K3H4 through the same boundary

## Open questions

- None yet
