# ADR-0002: Native Module Program Boundaries

- Status: accepted
- Date: 2026-06-27
- Context: global

## Problem

The native codebase still behaves like a monolith outside the existing K3H4 split. Ownership boundaries are broad, dependency direction is implicit, and extraction work can regress into internal include coupling if not constrained by one shared policy.

## Decision

Adopt a native module-program boundary model inspired by modules/k3h4 with these rules:

- Use domain-first modules; technical layers stay internal to each module.
- Enforce a strict module dependency DAG from the start; no temporary cycles.
- Each module exposes one public boundary only:
  - target-based linking
  - module-prefixed public headers
  - no external includes into module internals
- Execute migration as phased one-shot extraction per module:
  - each module migration is one-shot inside a bounded phase
  - the full program advances phase-by-phase on a stable integration trunk
- Choose extraction order by dependency pressure and volatility, not folder naming.
- Require a four-part phase gate for every extraction phase:
  - native configure/build
  - boundary compliance scan
  - targeted native test regex
  - API/UI smoke checks

## Consequences

- Positive:
  - Clear ownership and stable seams across native domains
  - Lower long-tail coupling risk by prohibiting cross-module internal includes
  - Safer migration cadence through phase-level gates and rollback points
- Negative:
  - Requires up-front module boundary taxonomy and enforcement scripts
  - May increase short-term build wiring and test rewiring churn
- Follow-up actions:
  - Create and maintain a native module split epic with phase-level stories
  - Add boundary compliance checks to CI and local validation flows
  - Keep context glossaries synchronized with module vocabulary

## Alternatives considered

- One global one-shot migration for all native modules: rejected due to rollback and blast-radius risk.
- Technical-layer modules first (for example all adapters together): rejected because it re-creates monolith seams under new folders.
