# Active Root Guardrails

## Objective

Prevent reintroduction of non-pivot workflow/spec sprawl while delivering Banana Engineer.

## Scope Guardrails

1. Product-first: prioritize `001-react-portal-game-client` deliverables before adding new exploratory specs.
2. Canonical CI harness only: keep `.github/workflows/banana.yml` as the single managed monorepo workflow entrypoint.
3. Archive boundary: store retired surfaces only under `.specify/legacy-baseline/**`; do not revive them in active roots.
4. Wiki boundary: keep wiki content as tutorials/runbooks, never runtime artifact dumping.
5. Contract-first safety: maintain extension preflight, confidence gate, and heartbeat requirements before orchestration.

## Change Admission Rules

- New spec under `.specify/specs/` requires explicit pivot relevance or dependency-unlock rationale.
- New workflow lane requires a clear bounded responsibility and must integrate into the canonical harness contract.
- Any temporary experiment must declare expiry and archive destination in its spec tasks/evidence.

## Review Checklist

- Does this change advance Banana Engineer UX/runtime delivery?
- Does this change avoid creating duplicate workflow surfaces?
- Does this change keep archival content outside active roots?
- Are confidence/heartbeat governance controls preserved?

## Escalation Rule

When a proposed addition does not clearly satisfy these guardrails, pause and request human approval before merging into active roots.
