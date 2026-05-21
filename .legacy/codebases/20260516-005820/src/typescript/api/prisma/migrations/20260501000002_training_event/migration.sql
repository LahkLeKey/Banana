-- Migration: 20260501000002_training_event
-- Spec 097: Event Sourcing for Training

CREATE TABLE "training_event" (
    "id"         TEXT         NOT NULL,
    "event_type" TEXT         NOT NULL,
    "model"      TEXT,
    "payload"    JSONB        NOT NULL,
    "actor"      TEXT,
    "created_at" TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT "training_event_pkey" PRIMARY KEY ("id")
);

CREATE INDEX "training_event_event_type_idx" ON "training_event"("event_type");
CREATE INDEX "training_event_model_idx"      ON "training_event"("model");
CREATE INDEX "training_event_created_at_idx" ON "training_event"("created_at");
