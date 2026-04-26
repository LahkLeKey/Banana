---
description: "Tasks for v2 TypeScript API regeneration"
---

# Tasks: TypeScript API (v2)

## Phase 0 — Ownership inventory

- [ ] T001 Enumerate routes; mark canonical owner (`007` or `008`) in `contracts/route-ownership.md`.
- [ ] T002 Identify duplicate or near-duplicate routes; pick canonical.

## Phase 1 — Boundary cleanup

- [ ] T010 Move handlers into `domains/<domain>/` folders.
- [ ] T011 [P] Move all wire schemas into `contracts/`.
- [ ] T012 [P] Reduce `shared/` to genuinely cross-domain utilities only.

## Phase 2 — Prisma reliability

- [ ] T020 Verify `prisma.config.ts` shape; ensure `schema.prisma` has no `datasource.url`.
- [ ] T021 Replace `prisma-migrate-status` helper invocations with direct `bunx prisma migrate status --schema prisma/schema.prisma`.
- [ ] T022 Add CI step: `bunx prisma migrate deploy` against an ephemeral DB.

## Phase 3 — Route migration / removal

- [ ] T030 Migrate routes to canonical owner; deprecate the other side.
- [ ] T031 Update frontend clients (cross-link to `009/010/011`).
- [ ] T032 Remove deprecated routes after consumers are migrated.

## Phase 4 — Training data lifecycle

- [ ] T040 Define corpus feedback contract (`text`, `label`, `source`) with typed validation errors.
- [ ] T041 [P] Implement auditable feedback persistence + bounded retrieval for curation workflows.
- [ ] T042 [P] Document training-data ownership and cutover responsibilities against `007`.

## Phase 5 — Model artifact runtime semantics

- [ ] T050 Load trained artifacts (`vocabulary`, `metrics`) with explicit fallback mode.
- [ ] T051 [P] Expose model metadata route including `source`, generated timestamp, and `recommended_threshold`.
- [ ] T052 [P] Ensure score responses include `threshold_source`, token-match fields, and typed no-signal rejection.

## Phase 6 — Chatbot banana-vs-not-banana contract

- [ ] T060 Keep chat session/message idempotency contract (`client_message_id`) deterministic.
- [ ] T061 [P] Align chatbot classification threshold semantics with `/not-banana/score`.
- [ ] T062 [P] Add route tests covering trained-model threshold behavior and duplicate-message replay.

## Phase 7 — Drift and release guards

- [ ] T070 Add CI drift checks for `data/not-banana/corpus.json`, training scripts, and runtime contract outputs.
- [ ] T071 Wire drift-check failures into release gates for this slice.
- [ ] T072 Record expected artifact schema/versioning in `contracts/`.

## Dependencies

- T001 blocks Phase 3.
- T020 must precede CI changes in T022.
- T040 blocks T050 and T060.
- T050 blocks T051/T052 and chat threshold parity in T061.
- T070 depends on completed runtime contract fields in T051/T052.
