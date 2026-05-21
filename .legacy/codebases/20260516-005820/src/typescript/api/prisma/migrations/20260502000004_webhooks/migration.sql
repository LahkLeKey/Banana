-- Migration: 20260502000004_webhooks
-- Spec 071: Outbound Webhook Delivery System

CREATE TABLE "WebhookSubscriber" (
    "id"            TEXT         NOT NULL,
    "url"           TEXT         NOT NULL,
    "events"        TEXT[]       NOT NULL,
    "signingSecret" TEXT         NOT NULL,
    "active"        BOOLEAN      NOT NULL DEFAULT true,
    "createdAt"     TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT "WebhookSubscriber_pkey" PRIMARY KEY ("id")
);

CREATE TABLE "WebhookDeliveryAttempt" (
    "id"           TEXT         NOT NULL,
    "subscriberId" TEXT         NOT NULL,
    "event"        TEXT         NOT NULL,
    "payload"      JSONB        NOT NULL,
    "statusCode"   INTEGER,
    "responseBody" TEXT,
    "success"      BOOLEAN      NOT NULL DEFAULT false,
    "attemptedAt"  TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT "WebhookDeliveryAttempt_pkey" PRIMARY KEY ("id")
);

CREATE INDEX "WebhookSubscriber_active_idx" ON "WebhookSubscriber"("active");
CREATE INDEX "WebhookDeliveryAttempt_subscriberId_attemptedAt_idx" ON "WebhookDeliveryAttempt"("subscriberId", "attemptedAt");
CREATE INDEX "WebhookDeliveryAttempt_event_success_idx" ON "WebhookDeliveryAttempt"("event", "success");

ALTER TABLE "WebhookDeliveryAttempt"
    ADD CONSTRAINT "WebhookDeliveryAttempt_subscriberId_fkey"
    FOREIGN KEY ("subscriberId") REFERENCES "WebhookSubscriber"("id")
    ON DELETE CASCADE ON UPDATE CASCADE;
