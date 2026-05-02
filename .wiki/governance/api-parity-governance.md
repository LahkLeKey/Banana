<!-- breadcrumb: Governance > API Parity Governance -->

# API Parity Governance

> [Home](../Home.md) › [Governance](README.md) › API Parity Governance

## What This Covers

This page explains how Banana enforces overlapping API behavior parity between:
- ASP.NET API on port 8080
- Fastify API on port 8081

Scope is only overlapping routes, not intentionally single-surface endpoints.

## Parity Gate Rules

A parity run passes only when each overlap route is either:
- fully aligned for route presence, status semantics, and required response fields, or
- covered by an active approved parity exception.

A parity run fails when unresolved drift remains in any mismatch type:
- `missing_route`
- `status_mismatch`
- `shape_mismatch`

## Core Evidence Artifacts

Parity evidence is written under:
- `.specify/specs/047-api-parity-governance/artifacts`

Primary files:
- `overlapping-routes.inventory.json`
- `parity-drift-report.json`
- `parity-exceptions.json`
- `parity-gate-result.json`

## Local Validation Commands

Inventory only:

```bash
bash scripts/validate-api-parity-governance.sh --inventory-only
```

Full parity check (non-strict shell exit):

```bash
bash scripts/validate-api-parity-governance.sh
```

Fail-closed strict check:

```bash
bash scripts/validate-api-parity-governance.sh --strict
```

## CI Integration Summary

Compose CI includes a dedicated lane named `api-parity` that:
- runs strict parity validation,
- records lane and job status,
- publishes parity artifacts for audit/debug,
- contributes to failure aggregate reporting.

## Exception Hygiene

Parity exceptions are temporary and exact-match only. Each exception should include:
- `route_key`
- `mismatch_type`
- `owner`
- `rationale`
- `approved_at`
- `expires_at`

Expired or malformed exceptions do not suppress failures.

## Fast Triage Checklist

1. Open `parity-gate-result.json` and inspect decision plus `unresolved_summary`.
2. Open `parity-drift-report.json` and sort by `mismatch_type`.
3. Confirm whether any exception is active, valid, and scoped correctly.
4. Resolve route/status/shape drift or add a time-bounded approved exception.
