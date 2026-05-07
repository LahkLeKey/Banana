# Tasks — 071 Outbound Webhook Delivery System

- [x] T001 — Add `WebhookSubscriber` and `WebhookDeliveryAttempt` Prisma models
- [x] T002 — Create migration `20260502000004_webhooks`
- [x] T003 — Implement `webhooksRoutes` in `src/routes/webhooks.ts`
- [x] T004 — POST `/webhooks/subscribers` — register subscriber, return one-time signing secret
- [x] T005 — GET `/webhooks/subscribers` — list subscribers (no secret exposed)
- [x] T006 — DELETE `/webhooks/subscribers/:id` — soft-deactivate subscriber
- [x] T007 — GET `/webhooks/deliveries` — paginated delivery attempt log
- [x] T008 — Implement `deliverWebhookEvent(prisma, event, payload)` with HMAC-SHA256 signing
- [x] T009 — Implement `signWebhookPayload` / `verifyWebhookSignature` helpers
- [x] T010 — Wire `webhooksRoutes` into `index.ts`
- [x] T011 — Timeout delivery HTTP calls at 10 s with `AbortSignal.timeout`
