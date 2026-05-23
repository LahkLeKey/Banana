# Steam Vendor Incident Brief

## Purpose

This brief is intended for Steam as a third-party distribution vendor. It focuses on infrastructure-level bypass risks and platform/control expectations, not internal implementation specifics.

## Incident Summary

- Reported concern: unapproved WASM engine payloads could be loaded in a beta distribution context.
- Risk class: accidental publish bypass and runtime artifact trust boundary violation.
- Potential impact: unauthorized code execution path via swapped or non-approved runtime artifacts.

## Vendor-Facing Threat Model

Primary concern is not source-code defects alone. The concern is control gaps across distribution and runtime infrastructure.

1. Artifact provenance bypass
- Runtime may consume artifacts outside approved release provenance.
- Missing or weak attestation/signing checks allow artifact substitution risk.

2. Distribution channel drift
- Beta/staging artifacts can be exposed beyond intended audience or path.
- Branch/depot misconfiguration can cause non-production artifacts to appear in user-facing channels.

3. CDN/cache and edge path inconsistency
- Caching behavior may serve stale or unexpected binaries.
- Path normalization inconsistencies can allow alternative artifact resolution.

4. Upload and publish authorization risk
- Insufficient separation between build, upload, and publish permissions increases accidental release risk.
- Lack of two-party approval on high-risk runtime artifacts increases bypass probability.

5. Runtime retrieval policy mismatch
- Platform-delivered artifacts may differ from expected allowlisted set at launch time.
- Absence of strict runtime integrity checks can permit fallback to unapproved payloads.

## Infra Bypass Indicators To Investigate

1. Branch/depot mapping anomalies for beta app/depot IDs.
2. Unexpected artifact hash deltas between build output and Steam-delivered payloads.
3. CDN cache keys serving mismatched artifact versions.
4. Manifest/package references to non-approved WASM paths.
5. Publish actions performed without required reviewer or policy gate.

## Required Infrastructure Controls

1. Artifact integrity and provenance
- Enforce immutable artifact hashing for engine runtime payloads.
- Require signed release manifests and verifiable artifact provenance.

2. Release channel isolation
- Hard-isolate beta and production depots/branches.
- Require explicit allowlists for branch-to-depot publish targets.

3. Deployment governance
- Enforce dual-approval or protected publish workflow for runtime engine artifacts.
- Require mandatory release checklist gate before vendor publish.

4. Runtime trust boundary controls
- Enforce strict runtime allowlist for approved artifact origins and paths.
- Fail closed when expected approved artifact identity is not met.

5. Observability and forensics
- Preserve publish event logs, actor identity, manifest IDs, and artifact hashes.
- Retain cache invalidation and edge-serving logs for incident reconstruction.

## Evidence Package For Vendor Review

Provide or request the following evidence for incident closure:

1. Build artifact hashes (pre-upload).
2. Steam-uploaded package/depot manifest identifiers.
3. Steam-delivered artifact hashes from beta retrieval path.
4. Branch/depot mapping snapshot at incident time.
5. Publish authorization and approval event logs.
6. CDN/cache invalidation timeline for affected assets.

Generate a local infrastructure evidence pack before outreach:

```bash
bash scripts/generate-steam-vendor-incident-evidence.sh
```

Generate a complete outreach bundle (evidence + email draft + attachment manifest):

```bash
bash scripts/prepare-steam-vendor-escalation-bundle.sh
```

Output location:

- `artifacts/security/steam-vendor/steam-vendor-incident-evidence-<timestamp>.md`
- `artifacts/security/steam-vendor/steam-vendor-incident-hashes-<timestamp>.txt`
- `artifacts/security/steam-vendor/steam-vendor-escalation-email-<timestamp>.md`
- `artifacts/security/steam-vendor/steam-vendor-attachment-manifest-<timestamp>.txt`

## Requested Steam-Side Actions

1. Validate beta branch/depot mapping and access policy for the affected app.
2. Confirm no cross-branch artifact bleed from beta into broader distribution.
3. Provide event trail for upload, publish, and cache propagation for impacted runtime assets.
4. Confirm recommended controls for strict artifact identity enforcement in Steam delivery workflows.

## Current Mitigation Status (High Level)

- Runtime artifact trust boundaries and automated security gates have been tightened.
- Ongoing focus: infra publish-path controls, artifact identity guarantees, and vendor-side distribution validation.

## Closure Criteria

Incident can be considered remediated only when all are true:

1. Vendor and publisher agree on artifact identity enforcement controls.
2. Branch/depot isolation is verified with evidence.
3. Publish governance controls are active for high-risk runtime assets.
4. Reproduction path for unapproved artifact loading is no longer possible.
5. Monitoring and audit trail are in place for future detection.
