-- Migration: 20260501000001_label_request
-- Spec 077: Active Learning Loop — label request queue

CREATE TABLE "label_request" (
    "id"                 TEXT         NOT NULL,
    "text"               TEXT         NOT NULL,
    "model"              TEXT         NOT NULL,
    "current_prediction" JSONB,
    "uncertainty"        DOUBLE PRECISION,
    "status"             TEXT         NOT NULL DEFAULT 'queued',
    "label"              TEXT,
    "labelled_by"        TEXT,
    "labelled_at"        TIMESTAMP(3),
    "created_at"         TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT "label_request_pkey" PRIMARY KEY ("id")
);

CREATE INDEX "label_request_status_idx" ON "label_request"("status");
CREATE INDEX "label_request_model_idx"  ON "label_request"("model");
