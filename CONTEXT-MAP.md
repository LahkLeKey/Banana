# Context Map

This repository uses a multi-context domain documentation layout.

Use this file to route architecture, bug-diagnosis, and implementation work to the correct domain context before reading any context glossary or ADRs.

## Global rules

- Start with the context whose primary code path is being changed.
- If work spans contexts, read all affected context files listed below.
- Prefer context-local ADRs first, then global ADRs at docs/adr.
- If a context file does not exist yet, treat this map entry as the canonical location to create it.

## Contexts

| Context | Purpose boundary | Primary code roots | Context file | Context ADR folder |
| --- | --- | --- | --- | --- |
| Native C ABI | Public C-facing ABI surface, FFI contracts, exported headers, binary compatibility rules | src/native/include | src/native/CONTEXT.md | src/native/docs/adr |
| Native engine runtime | Internal native runtime behavior, engine internals, model/runtime integration details | src/native/engine | src/native/engine/CONTEXT.md | src/native/engine/docs/adr |
| K3H4 core module | K3H4 native core implementation and tests, exposed behind one public module boundary | modules/k3h4/native | modules/k3h4/CONTEXT.md | modules/k3h4/docs/adr |
| API service | Backend application/domain/use-case logic, HTTP routes, persistence, service composition | src/typescript/api | src/typescript/api/CONTEXT.md | src/typescript/api/docs/adr |
| Client applications | User-facing client behavior, UI interaction flows, app shell concerns, web/runtime integration | src/typescript/react | src/typescript/react/CONTEXT.md | src/typescript/react/docs/adr |
| Shared contracts and libraries | Cross-context contracts, shared UI/resilience helpers, reusable package APIs used by API and clients | src/typescript/shared, src/typescript/packages | src/typescript/shared/CONTEXT.md | src/typescript/shared/docs/adr |

## Cross-context seams

- Native C ABI <-> API service: FFI boundary and payload/layout contracts.
- K3H4 core module <-> Native engine runtime: runtime integration via the k3h4 public module API only.
- K3H4 core module <-> API service: authoritative analytics behavior through stable native integration contracts.
- API service <-> Client applications: HTTP/schema contract and runtime expectations.
- Shared contracts and libraries <-> API/client: shared types, compatibility, and package-level API guarantees.

## Global ADR location

Place system-wide decisions that affect multiple contexts in docs/adr.

## ADR index

- Global ADRs: docs/adr
- Native C ABI ADRs: src/native/docs/adr
- Native engine runtime ADRs: src/native/engine/docs/adr
- K3H4 core module ADRs: modules/k3h4/docs/adr
- API service ADRs: src/typescript/api/docs/adr
- Client applications ADRs: src/typescript/react/docs/adr
- Shared contracts and libraries ADRs: src/typescript/shared/docs/adr

## Maintenance

When adding a new bounded area, add it to this map with:

1. Purpose boundary
2. Primary code roots
3. Context file path
4. Context ADR folder path
