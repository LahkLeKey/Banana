-- Migration: 20260502000003_background_jobs
-- Spec 070: Background Job Runner (Durable Queue)

CREATE TABLE "BackgroundJob" (
    "id"          TEXT         NOT NULL,
    "kind"        TEXT         NOT NULL,
    "payload"     JSONB        NOT NULL,
    "status"      TEXT         NOT NULL DEFAULT 'pending',
    "attempts"    INTEGER      NOT NULL DEFAULT 0,
    "maxAttempts" INTEGER      NOT NULL DEFAULT 3,
    "runAfter"    TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "startedAt"   TIMESTAMP(3),
    "finishedAt"  TIMESTAMP(3),
    "error"       TEXT,
    "createdAt"   TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "updatedAt"   TIMESTAMP(3) NOT NULL,
    CONSTRAINT "BackgroundJob_pkey" PRIMARY KEY ("id")
);

CREATE INDEX "BackgroundJob_status_runAfter_idx" ON "BackgroundJob"("status", "runAfter");
CREATE INDEX "BackgroundJob_kind_status_idx" ON "BackgroundJob"("kind", "status");
