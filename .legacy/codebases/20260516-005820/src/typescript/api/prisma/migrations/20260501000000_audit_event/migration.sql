-- Migration: audit_event table (spec #069)
-- Created: 2026-05-01

CREATE TABLE "audit_event" (
    "id"          TEXT         NOT NULL DEFAULT gen_random_uuid()::text,
    "actor"       TEXT,
    "action"      TEXT         NOT NULL,
    "resource"    TEXT         NOT NULL,
    "resource_id" TEXT,
    "metadata"    JSONB,
    "ip_address"  TEXT,
    "created_at"  TIMESTAMPTZ  NOT NULL DEFAULT now(),

    CONSTRAINT "audit_event_pkey" PRIMARY KEY ("id")
);

CREATE INDEX "audit_event_action_idx"     ON "audit_event" ("action");
CREATE INDEX "audit_event_actor_idx"      ON "audit_event" ("actor");
CREATE INDEX "audit_event_created_at_idx" ON "audit_event" ("created_at");
