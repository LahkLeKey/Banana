# DAL Outcome Matrix

| Outcome Category | Trigger Condition | Native Outcome Expectation | Success? |
|---|---|---|---|
| `unconfigured` | `BANANA_PG_CONNECTION` missing or empty | Deterministic non-success with remediation | No |
| `dependency_unavailable` | DAL dependency/link/runtime path unavailable | Deterministic non-success, no synthetic success payload | No |
| `query_failure` | DAL query execution cannot complete | Deterministic non-success, query-failure classification | No |
| `success` | Valid config and successful query execution | Existing successful behavior remains compatible | Yes |
