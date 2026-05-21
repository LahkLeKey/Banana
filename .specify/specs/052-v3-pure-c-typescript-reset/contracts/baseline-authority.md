# Contract: V3 Baseline Authority

## Purpose

Define what may govern active V3 planning decisions after the reset.

## Authoritative inputs

| Input | Role in baseline authority | Current evidence |
|---|---|---|
| `.specify/specs/052-v3-pure-c-typescript-reset/spec.md` | Governing scope, supersession, and success criteria | Active reset spec |
| `.specify/specs/052-v3-pure-c-typescript-reset/plan.md` | Execution strategy and repository structure | Active implementation plan |
| `.specify/specs/052-v3-pure-c-typescript-reset/research.md` | Source-first and lineage-only decisions | Active rationale |
| `.specify/specs/052-v3-pure-c-typescript-reset/data-model.md` | Validation entities and rules | Active schema |
| `.specify/specs/052-v3-pure-c-typescript-reset/codebase-scan.md` | Retained capability and evidence inventory | Active repository scan |
| `.specify/specs/README.md` | Active baseline registry | Lists Spec 052 as sole active authority |
| `.specify/feature.json` | Active feature pointer | Points to Spec 052 |
| `src/native/**` and `src/typescript/**` | Live runtime truth | Scaffold-only source baseline |

## Non-authoritative inputs

| Input | Why it is not authoritative |
|---|---|
| `.specify/legacy-baseline/**` | Archived lineage retained for history and migration notes only |
| `.legacy/**` | Historical code snapshots, not live runtime truth |
| Stale viewport-first assumptions | Explicitly excluded from baseline correctness |
| Historical heartbeat or planning claims without matching source evidence | Cannot override live repository state |

## Decision rule

When active source evidence conflicts with archived or speculative material:

1. Prefer the live source path and active Spec 052 artifacts.
2. Demote the conflicting material to lineage-only status.
3. Record the stale-path decision in `research.md`, `codebase-scan.md`, or `heartbeat-log.md` when it affects downstream planning.

## Scope gate evidence

A retained capability is valid for V3 only when it is:

- gameplay-relevant,
- customer-facing in a runtime channel, and
- backed by at least one live runtime entry path.

The review evidence for that gate lives in:

- `codebase-scan.md`
- `v3-scope-gate-checklist.md`
- `heartbeat-log.md`

## Required downstream citation

Every downstream planning decision must cite:

- at least one authoritative input,
- exactly one owning execution lane, and
- one lane-local validation expectation tied to the current scaffold baseline.
