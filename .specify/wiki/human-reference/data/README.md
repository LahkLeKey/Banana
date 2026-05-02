# Data

> [Home](../Home.md) › Data

Migration discipline, coverage policies, and data contracts.

## Pages In This Section

| Page | Description |
|------|-------------|
| [Migration Discipline](migration-discipline.md) | Prisma migration workflow and backward-compat rules |
| [Coverage Exceptions](coverage-exceptions.md) | How to add and manage coverage 80% exceptions |

## Key Rules

- All schema changes go through Prisma migrations.
- Destructive SQL (DROP, TRUNCATE) requires CI review gate.
- Coverage exceptions are temporary — set expiry dates and remove promptly.
