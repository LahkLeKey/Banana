# Ownership Matrix: 007 API MMO Recenter

| Route namespace | Owner bounded context | Notes |
|---|---|---|
| `/v1/gameplay/*` | gameplay-session-orchestration | Authoritative DX12 orchestration flows |
| `/v1/player/*` | player-identity-account | Player-facing web account and shared truth surfaces |
| `progression/inventory settlement` | progression-inventory | Exactly-once mutation settlement and ledgers |
| `insights projections` | player-insights-web | Read-only, derived from authoritative records |

Validation command:

```bash
bash scripts/validate-api-parity-governance.sh --strict
```
