<!-- breadcrumb: Architecture > Domain Model -->

# Domain Model

> [Home](../Home.md) › [Architecture](README.md) › Domain Model

Related pages: [Overview](overview.md), [Native Wrapper ABI](native-wrapper-abi.md)

> **Status:** Stub — expand with DDD entity map and aggregate boundaries.

## Core Domain Areas

| Area | Location | Responsibility |
|------|----------|----------------|
| Cultivation | `src/native/core/domain/` | Growing stage and harvest metadata |
| Processing | `src/native/core/domain/` | Processing and packaging calculations |
| Supply Chain | `src/native/core/domain/` | Logistics and distribution |
| Ripeness | `src/native/core/domain/` | Ripeness telemetry and prediction |
| DAL projections | `src/native/core/dal/domain/` | Persistence and aggregate projection |

## Read Next

- [How A Request Works](../getting-started/how-a-request-works.md) — see how domain objects flow through a request.
- [docs/banana-ddd.md](../../docs/banana-ddd.md) — full domain-driven design documentation.
