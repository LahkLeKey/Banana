# Steam Storefront Readiness Checklist

Target AppID: `4767150`

Note: This checklist tracks public-facing Steam store readiness, not binary packaging.
Use `steam-storefront-copy.md` as the canonical claim target.

## Identity and Copy

- [ ] Store short description matches the storefront copy contract.
- [ ] About section matches the storefront copy contract.
- [ ] Genre, developer, publisher, and planned release date are aligned.
- [ ] Store page title is consistent across Steam, build metadata, and repo artifacts.
- [ ] Store page uses the exact current claim strings documented in the storefront contract.

## Controller Support Lane

- [ ] Validate the declared controller support level against actual in-game input behavior.
- [ ] Confirm controller prompts and navigation work in the visible gameplay flow.
- [ ] Verify store page wording matches the support level shipped in the build.
- [ ] Confirm the page remains at Partial Controller Support unless runtime input coverage changes.

## AI Disclosure Lane

- [ ] Confirm the AI/content disclosure sentence matches the current implementation.
- [ ] Verify the public disclosure line matches repo policy and release materials.
- [ ] Record the service/model/deployment claim in release evidence.
- [ ] Confirm the disclosure does not overstate unimplemented generation paths.
- [ ] Use the exact current disclosure line documented in the storefront contract.

## System Requirements Lane

- [ ] Reconcile minimum OS, CPU, memory, GPU, DirectX, network, and storage claims with the build target.
- [ ] Validate the minimum spec against the playable candidate on the lowest supported environment.
- [ ] Keep the storefront system requirements synchronized with release notes and packaging docs.
- [ ] Use the exact current minimum requirement lines documented in the storefront contract.

## Store Capsule And Asset Lane

- [ ] Verify logo, capsule, hero, background, and screenshot assets exist in `artifacts/steam-placeholders/`.
- [ ] Confirm the store page image set matches the current product branding.
- [ ] Ensure screenshot evidence shows the intended gameplay flow and not placeholder-only scenes.
- [ ] Check that asset dimensions align with Steam store upload expectations.
- [ ] Track the current asset inventory in `steam-storefront-assets.md`.

## Public Claim Alignment

- [ ] Confirm the store page language about open source, persistent world, and MMOARPG scope matches the shipped product.
- [ ] Confirm the copy about class builds, gear upgrades, skill combinations, solo runs, and party runs matches the current feature set.
- [ ] Verify the public page does not advertise unsupported runtime behavior.

## Vendor Infrastructure Bypass Lane

- [ ] Verify beta and production branch/depot mappings are isolated and reviewed for accidental publish paths.
- [ ] Confirm runtime artifact hash parity between build outputs, uploaded depots, and Steam-delivered payloads.
- [ ] Confirm publish governance requires explicit approval for runtime engine artifact changes.
- [ ] Validate that CDN/cache invalidation was executed for affected runtime artifacts after remediation.
- [ ] Record upload/publish actor timeline and manifest identifiers for incident forensics.
- [ ] Confirm release communication to vendor uses infra-first framing from `docs/steam-vendor-incident-brief.md`.
- [ ] Confirm closure criteria from `docs/steam-vendor-incident-brief.md` are met before storefront signoff.
- [ ] Generate and attach vendor evidence pack via `bash scripts/generate-steam-vendor-incident-evidence.sh`.

## Signoff Evidence

- [ ] Capture final store page screenshots after copy and asset updates.
- [ ] Record the exact AI disclosure text used on the page.
- [ ] Record the exact minimum system requirements text used on the page.
- [ ] Link the completed storefront checklist to the Steam UAT evidence bundle.
- [ ] Confirm the final store page text is an exact match for the storefront contract or an approved revision.
- [ ] Attach the completed vendor incident brief `docs/steam-vendor-incident-brief.md` to the Steam communication thread.
- [ ] Attach infrastructure evidence set: build hashes, depot manifests, delivered hashes, branch/depot snapshot, publish logs, and cache timeline.
