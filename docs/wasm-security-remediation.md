# WASM Security Remediation (Archived)

## Status

This runbook is archived.

As of 2026-05-23, Banana no longer ships WASM assets as part of the active
product delivery path. The Steam Windows desktop client (DirectX12 runtime)
is the primary executable deliverable, while the website remains a marketing
and account-entry surface.

## What changed

- WASM asset directories and build scripts were removed from active workflows.
- CI lanes dedicated to WASM build/pentest execution were removed.
- Web routing was shifted to marketing/login surfaces instead of gameplay runtime.

## Security posture going forward

- Steam OpenID + API JWT session orchestration remains active.
- Session persistence is backed by Neon/PostgreSQL when configured.
- Desktop runtime hardening and distribution integrity checks remain in scope.

## Historical reference only

The prior WASM incident details and checklist language are retained in git
history for audit traceability, but this document should not be used as an
active release gate for current delivery lanes.
