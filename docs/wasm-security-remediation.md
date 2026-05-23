# WASM Security Remediation Runbook

## Scope

This runbook captures full remediation for the WASM loading incident, including runtime guardrails, penetration coverage, CI enforcement, and release evidence.

For third-party vendor escalation focused on Steam distribution and infrastructure bypass risk, use `docs/steam-vendor-incident-brief.md` as the primary report.

## Remediation Controls

1. API runtime hardening
- File: `src/typescript/api/wasm/engine-binding.ts`
- Control: `resolveTrustedEngineScriptUrl()` enforces same-origin, strips credentialed URLs, and restricts path to `/wasm/engine.js`.
- Control: `initializeEngine()` fails if canvas is missing or module ABI is incomplete.

2. Desktop runtime hardening
- File: `src/typescript/electron/main.js`
- Control: `isAllowedRendererUrl()` allows only `file://` or loopback `http(s)` URLs.
- Control: `resolveStartUrl()` rejects unsafe `BANANA_DESKTOP_START_URL` values.
- Control: `will-navigate` and `setWindowOpenHandler` block disallowed navigation/window targets.

3. Penetration regression coverage
- Files:
  - `src/typescript/api/wasm/engine-binding.security.test.ts`
  - `src/typescript/api/wasm/engine-binding.pentest.test.ts`
- Control: URL trust and initialization attack-surface tests cover cross-origin/script-path abuse, traversal-like payloads, malformed module surface, and missing-canvas failure.

4. Generated asset policy hardening
- File: `src/typescript/react/scripts/prepare-procedural-assets.mjs`
- Control: explicit policy/mode contract:
  - `BANANA_GENERATED_ASSET_POLICY=auto|strict`
  - `BANANA_GENERATED_ASSET_COMPILE_MODE=skip|auto|force`
- Control: Vercel-safe default sets compile mode to `skip` when `VERCEL=1`.
- Control: strict mode fails fast when generated assets are required.

5. CI hard gates and evidence
- File: `.github/workflows/banana.yml`
- Control: blocking lane `Security / WASM penetration tests`.
- Control: persistent report artifact upload at `artifacts/ci/wasm-security`.
- Control: `Build / TypeScript smoke` includes strict generated-assets contract check with native compile skipped intentionally.

## Verification Commands

Run from repository root.

1. API WASM penetration suite

```bash
cd src/typescript/api
bun install --frozen-lockfile || bun install
bun run test:wasm-pentest
```

Expected outcome:
- All WASM security/pentest tests pass.

2. Generated assets strict contract (no native compile)

```bash
cd src/typescript/react
BANANA_GENERATED_ASSET_POLICY=strict \
BANANA_GENERATED_ASSET_COMPILE_MODE=skip \
bun scripts/prepare-procedural-assets.mjs
```

Expected outcome:
- Logs include `policy=strict` and `compileMode=skip`.
- Script reports copied generated assets.

3. React production build with Vercel-like environment

```bash
cd src/typescript/react
VERCEL=1 bun run build
```

Expected outcome:
- Native procedural compiler step is skipped.
- Build completes successfully.

## Before And After Verification Snapshot

Verification date: 2026-05-22

1. Incident state before remediation
- Untrusted or unapproved WASM bootstrap paths could be accepted.
- Desktop entry/navigation constraints were not strict enough for this threat model.
- No dedicated WASM penetration regression gate existed in CI.
- No persistent CI evidence artifact was required for WASM security validation.

2. Current state after remediation
- API bootstrap URL guard enforces same-origin `/wasm/engine.js` and blocks credentialed/cross-origin input.
- Electron runtime only allows `file://` and loopback `http(s)` renderer targets.
- Dedicated WASM penetration suite is enforced as a blocking CI lane.
- WASM security output and report are uploaded as CI artifacts.
- Generated asset contract is strict in CI while Vercel/non-native paths remain stable via compile skip mode.

3. Fresh verification results (2026-05-22)
- `bun run test:wasm-pentest`: PASS (`9 pass`, `0 fail`).
- `BANANA_GENERATED_ASSET_POLICY=strict BANANA_GENERATED_ASSET_COMPILE_MODE=skip bun scripts/prepare-procedural-assets.mjs`: PASS (strict policy + compile skip confirmed).
- `VERCEL=1 bun run build`: PASS (production build succeeded, modules transformed, build completed).

Remediation report readiness: READY.

## CI Evidence Checklist

For each remediation PR/release:

1. `Banana-Monorepo` passes.
2. `Security / WASM penetration tests` lane is green.
3. Artifact `wasm-security-report-<run_id>` is present.
4. Artifact contains:
- `wasm-pentest-output.txt`
- `wasm-pentest-report.md`

## Emergency Containment Procedure

If suspicious WASM bootstrap behavior is reported:

1. Block release merge until `Banana-Monorepo` is green.
2. Re-run API pentest suite locally and in CI.
3. Verify API URL trust guard and Electron URL allowlist files remain unchanged.
4. Verify workflow still uploads WASM security artifact report.
5. Require approval from runtime owner before merge.

## Remediation Recovery Checklist

Use this list before reporting remediation completion externally.

1. Confirm API guardrails are present in `src/typescript/api/wasm/engine-binding.ts`.
2. Confirm Electron URL allowlist protections are present in `src/typescript/electron/main.js`.
3. Confirm pentest suites pass locally via `bun run test:wasm-pentest`.
4. Confirm workflow lane `Security / WASM penetration tests` is green in `Banana-Monorepo`.
5. Confirm artifact bundle `wasm-security-report-<run_id>` exists and includes both report and raw output files.
6. Confirm generated-assets strict policy check passes in `Build / TypeScript smoke`.
7. Confirm Vercel-like build path (`VERCEL=1`) skips native compiler step and still produces a successful build.

## External Security Update Template

Use or adapt this message when replying to internal/external security contacts.

Subject: Remediation Update: Accidental Publish Bypass (WASM Remote)

Hello Security Team,

Following the report of unapproved WASM loading risk, we completed remediation and added hard gates.

Completed actions:

1. Restricted WASM bootstrap script loading to same-origin `/wasm/engine.js` and blocked credentialed/cross-origin URLs.
2. Restricted Electron renderer start/navigation targets to `file://` and loopback `http(s)` only.
3. Added penetration regression suite for WASM bootstrap attack paths.
4. Added blocking CI lane for WASM penetration tests.
5. Added CI artifact evidence (`wasm-security-report-<run_id>`) with raw test output and structured report.
6. Added strict generated-asset contract enforcement in CI while keeping Vercel/non-native deployment builds stable.

Current status:

- Local pentest suite: passing.
- CI security lane: required and blocking.
- Deployment build path: validated with Vercel-like environment.

Residual risk tracking:

- Native procedural compiler availability is environment-dependent; strict enforcement is handled in dedicated CI policy mode.

Next actions:

1. Continue monitoring workflow security artifacts for regression signals.
2. Keep merge gate on `Banana-Monorepo` as mandatory.
3. Escalate any future WASM loader contract drift as a release blocker.

Regards,

Banana Engineer Team

## Notes

- Use strict generated-asset policy in CI lanes where deterministic artifact presence is required.
- Keep Vercel/non-native build channels on skip-compile mode unless native toolchain availability is guaranteed.
