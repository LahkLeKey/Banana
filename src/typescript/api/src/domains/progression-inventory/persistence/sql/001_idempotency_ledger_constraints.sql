CREATE TABLE IF NOT EXISTS gameplay_idempotency_settlements (
  scope TEXT NOT NULL,
  idempotency_key TEXT NOT NULL,
  payload_hash TEXT NOT NULL,
  response_json JSONB NOT NULL,
  created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  PRIMARY KEY (scope, idempotency_key)
);

CREATE TABLE IF NOT EXISTS gameplay_mutation_ledger (
  ledger_entry_id UUID PRIMARY KEY,
  session_id TEXT NOT NULL,
  player_id TEXT NOT NULL,
  idempotency_key TEXT NOT NULL,
  mutation_type TEXT NOT NULL,
  mutation_payload JSONB NOT NULL,
  recorded_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  UNIQUE (player_id, idempotency_key)
);
