Subject: Escalation: Accidental Publish Bypass Risk (WASM Runtime Artifact Trust)

Hello Steam Security / Steamworks Support,

We are escalating a beta-channel incident involving possible unapproved WASM runtime artifact loading.

Focus area: infrastructure and distribution trust controls.

Summary:
- Risk class: accidental publish bypass and runtime artifact trust-boundary violation.
- Concern: runtime artifact identity may drift across build, upload, and delivery paths.
- Impact: potential non-approved runtime artifact resolution.

Request for vendor-side validation:
1. Validate beta/production branch and depot isolation for AppID 4767150.
2. Confirm no cross-branch/depot artifact bleed in the incident window.
3. Provide upload/publish event trail with actor IDs and manifest/depot identifiers.
4. Provide cache/CDN invalidation and propagation timeline for affected artifacts.
5. Confirm recommended Steam controls for strict artifact identity enforcement.

Attached evidence:
- Infra incident evidence report
- Artifact hash manifest (build-side)
- Vendor incident brief and closure criteria

Thank you.

Banana Engineer Team
