# Feature Specification: Deployment Host Inventory Spike (158)

**Feature Branch**: `feature/158-deployment-host-inventory-spike`
**Created**: 2026-05-02
**Status**: Ready for implementation
**Input**: Live host state indicates banana.engineer domain configured but no active production deployment, plus active Fly.io organization capacity and app state.

## Problem Statement

Deployment state is currently spread across dashboards and ad hoc terminal checks. The team needs a repeatable, script-driven inventory baseline for Vercel and Fly.io so follow-up implementation and QA specs can consume objective evidence instead of screenshots.

## In Scope

- Define the required inventory command surface for Vercel and Fly.io.
- Capture production deployment, domain assignment, and machine-capacity state into machine-readable artifacts.
- Document known blocked states and required remediation inputs.
- Produce a research artifact that recommends implementation and QA follow-up boundaries.

## Out of Scope

- Triggering production deployment changes.
- Domain DNS changes or SSL reconfiguration.
- Runtime code changes in API/frontend services.

## Success Criteria

- Host inventory commands and expected output fields are defined and validated for both Vercel and Fly.io.
- A machine-readable inventory artifact format is agreed for use in follow-up implementation and QA specs.
- Blocked states include explicit remediation actions and owner expectations.

## Assumptions

- Vercel CLI and Fly CLI are available to operators.
- Authenticated access to the target Vercel team and Fly organization is available during validation.
