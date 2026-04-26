# Feature Specification: TypeScript API (v2)

**Feature Branch**: `008-typescript-api`  
**Created**: 2026-04-25  
**Status**: Draft  
**Parent**: [`005-v2-regeneration`](../005-v2-regeneration/spec.md)  
**Baseline**: [`legacy-baseline/typescript-api.md`](../../legacy-baseline/typescript-api.md)

## Summary

Regenerate the Fastify + Prisma TypeScript API with clear domain boundaries, a
documented relationship to the .NET API (`007`), and reliable Prisma migration
ergonomics. Decide canonical-API responsibility per route group and remove
overlap. Include explicit contracts for banana vs not-banana training data,
trained-model artifact usage, and chatbot session/message behavior so runtime
classification remains deterministic and auditable.

## User Scenarios & Testing

### User Story 1 — Canonical API ownership per route (Priority: P1)

As a frontend developer, I know which API (`007` ASP.NET vs `008` Fastify)
owns each route group, and that mapping is documented and enforced.

### User Story 2 — Reliable Prisma migrations (Priority: P1)

As a maintainer, `bunx prisma migrate status --schema prisma/schema.prisma`
runs cleanly from `src/typescript/api`, and migrations are reproducible across
local + CI.

### User Story 3 — Bun-native dev loop (Priority: P2)

As a developer, the dev loop is Bun end-to-end (install, run, test); no
accidental npm regressions.

### User Story 4 — Training data ingestion and artifact handoff (Priority: P1)

As an ML maintainer, I can ingest banana/not-banana corpus feedback, audit it,
and hand off deterministic training artifacts that runtime scoring and chat can
consume without format drift.

**Acceptance Scenarios**:

1. **Given** valid corpus feedback (`text`, `label`, `source`), **When**
	submitted, **Then** the API accepts it, records a stable ID + timestamp,
	and preserves an audit trail for training curation.
2. **Given** invalid feedback shape or unsupported labels, **When** submitted,
	**Then** the API returns typed `invalid_argument` details.
3. **Given** trained artifacts are available, **When** score/chat endpoints run,
	**Then** runtime metadata reports artifact source, generated timestamp, and
	recommended threshold.

### User Story 5 — Banana vs not-banana chatbot semantics (Priority: P1)

As an app user, I can ask chatbot questions about whether something is a banana,
and receive deterministic responses aligned with not-banana score semantics and
model thresholds.

**Acceptance Scenarios**:

1. **Given** the same session + `client_message_id`, **When** a message is
	retried, **Then** the API returns an idempotent duplicate response.
2. **Given** the same input text sent to score and chat classification logic,
	**When** evaluated, **Then** both use the same model source and threshold
	semantics.
3. **Given** input text with no known banana/not-banana signals, **When**
	scored, **Then** the API returns typed `invalid_argument` instead of silently
	classifying.

### Edge Cases

- A route exists in both APIs → spec MUST mark one canonical and document the cutover.
- Prisma datasource URL appears in `schema.prisma` → fail lint; configuration must live in `prisma.config.ts` per Prisma 7.
- Trained artifact missing/corrupt at runtime → API MUST declare fallback model source explicitly.
- Generic stopwords dominate corpus vocabulary → scoring/chat must ignore low-information tokens.
- Mixed threshold sources (request override vs model default) → response MUST declare `threshold_source`.

## Requirements

### Functional Requirements

- **FR-001**: v2 MUST document per-route ownership between `007` and `008`.
- **FR-002**: v2 MUST keep `prisma.config.ts` `defineConfig({ datasource: { url: env("DATABASE_URL") } })` pattern.
- **FR-003**: v2 MUST NOT include `datasource.url` in `schema.prisma`.
- **FR-004**: v2 MUST keep Bun as the package manager (`bun.lock` present, no `package-lock.json`).
- **FR-005**: Migrations MUST run via `bunx prisma migrate <cmd> --schema prisma/schema.prisma` from `src/typescript/api`.
- **FR-006**: v2 MUST consolidate `apps/`, `domains/`, `contracts/`, `server/`, `shared/` boundaries with documented rules.
- **FR-007**: Test suite under `tests/` MUST cover canonical routes.
- **FR-008**: v2 MUST define and validate banana/not-banana training feedback payloads (`text`, `label`, `source`) with typed error responses for invalid input.
- **FR-009**: v2 MUST persist accepted feedback in an auditable training-data trail and expose bounded retrieval for curation.
- **FR-010**: v2 MUST define a stable training-artifact contract (including vocabulary + metrics metadata) consumed by runtime scoring/chat.
- **FR-011**: `/not-banana/score` MUST load trained artifacts when available and declare explicit fallback behavior when unavailable.
- **FR-012**: Score responses MUST include `label`, `banana_score`, `threshold`, `threshold_source`, token-match fields, and model metadata (`source`, `recommended_threshold`, metrics).
- **FR-013**: Score requests with no known banana/not-banana signal tokens MUST return typed `invalid_argument` instead of defaulting to banana.
- **FR-014**: Chat session/message routes MUST support idempotent `client_message_id` replay semantics and deterministic response content.
- **FR-015**: Chat classification logic MUST stay contract-aligned with `/not-banana/score` model loader and threshold semantics.
- **FR-016**: v2 MUST enforce drift checks across training corpus, training script outputs, and runtime contract expectations in CI.
- **FR-017**: Route ownership docs MUST include corpus-feedback, score, model-metadata, and chatbot routes with explicit ownership versus `007`.

### Hard contracts to preserve

- `DATABASE_URL` env var.
- Bun as package manager.
- `prisma.config.ts` datasource pattern.
- Existing migration history (cannot re-baseline silently).
- `BANANA_NOT_BANANA_MODEL_PATH` optional override for runtime artifact loading.
- `BANANA_CORPUS_AUDIT_PATH` and `BANANA_CORPUS_AUDIT_DISABLED` behavior.
- Training artifact metadata contract used by score/chat (`metrics.min_signal_score`, generated timestamp, source).

### Pain points addressed

- Two APIs without canonical ownership → documented (FR-001).
- Loose `apps/`/`domains/`/`contracts/` boundaries → consolidated (FR-006).
- `prisma-migrate-status` undefined-path failure → use direct `bunx` invocation (FR-005).
- Training-data ingestion and runtime classification previously drifted → now explicit contract (FR-008 through FR-016).
- Score/chat threshold behavior diverged under mixed model states → now aligned + testable (FR-012, FR-015).

### Key Entities

- **Route ownership table**: maps each route group to `007` or `008`.
- **Prisma migration**: timestamped SQL change, never rewritten.
- **Training feedback record**: `{id, text, label, source, created_at, persistence}`.
- **Training artifact bundle**: vocabulary + metrics payload consumed by runtime classifier and chatbot.
- **Classifier decision payload**: `{label, banana_score, threshold, threshold_source, model metadata}`.
- **Chat session contract**: session + message IDs with idempotent replay guarantees.

## Success Criteria

- **SC-001**: Per-route ownership table exists in this spec or `contracts/`.
- **SC-002**: `bunx prisma migrate status --schema prisma/schema.prisma` exits 0 on a clean DB.
- **SC-003**: Bun is the only PM artifact (no `package-lock.json`).
- **SC-004**: Boundaries (`apps/` vs `domains/` vs `contracts/`) documented in `plan.md`.
- **SC-005**: Training feedback accepts valid banana/not-banana entries and rejects invalid payloads with typed errors.
- **SC-006**: Runtime model metadata endpoint returns `source`, `recommended_threshold`, and metrics when artifacts exist.
- **SC-007**: Score endpoint returns typed `invalid_argument` for no-signal input.
- **SC-008**: Same-session duplicate chatbot messages (same `client_message_id`) return idempotent duplicate payloads.
- **SC-009**: Score and chat responses report aligned threshold semantics for the same trained artifact.
- **SC-010**: CI drift checks fail when corpus/training-runtime contracts diverge.
