# Status Mapping Matrix

| Native Outcome Category | Expected Native Status Class | Managed HTTP Class | Error Body Expectation |
|---|---|---|---|
| `unconfigured` | `DbNotConfigured` (or mapped equivalent) | 503 | Error code indicates missing config and remediation mentions `BANANA_PG_CONNECTION` |
| `dependency_unavailable` | deterministic non-success status (DAL unavailable class) | 503 or 500 (per final mapping decision) | Error code is deterministic and non-success; no synthetic success content |
| `query_failure` | `DbError` (or mapped equivalent) | 500 | Error code indicates db failure class; deterministic schema |
| `success` | `Ok` | 200 | Existing successful payload compatibility retained |

## Mapping Ownership

- Native emits status outcome category.
- ASP.NET pipeline maps status to HTTP/error contract.
