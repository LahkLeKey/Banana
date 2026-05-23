# Steam Vendor Incident Evidence Pack

- GeneratedAt: 2026-05-22T21:38:44-05:00
- RepoBranch: hotfix/wasm-pentest-hardening
- RepoCommit: 02930ecb4590cad00c62065aa8eb6a0b639b7a9b
- HashManifest: /c/Github/Banana/artifacts/security/steam-vendor/steam-vendor-incident-hashes-20260522-213843.txt

## Infrastructure-Focused Evidence

This package is focused on distribution and artifact trust infrastructure.

1. Artifact provenance
- Local artifact hashes are recorded in the hash manifest file above.
- Compare these hashes to Steam-delivered payload hashes during incident review.

2. Distribution channel mapping (Steam-side required)
- BetaBranchName: <fill-from-steamworks>
- ProductionBranchName: <fill-from-steamworks>
- AffectedDepotIds: <fill-from-steamworks>
- IncidentWindowUTC: <fill-from-steamworks>

3. Publish governance (Steam-side required)
- UploadActorIds: <fill-from-steamworks>
- PublishActorIds: <fill-from-steamworks>
- ApprovalPolicyObserved: <fill-from-steamworks>

4. Cache/CDN timeline (Steam-side required)
- CacheInvalidationRequestedAt: <fill-from-steamworks>
- CacheInvalidationCompletedAt: <fill-from-steamworks>
- EdgePropagationNotes: <fill-from-steamworks>

5. Closure checks
- Branch/depot isolation verified with evidence.
- Hash parity confirmed between build/upload/delivery.
- Unapproved WASM artifact load path is not reproducible.

## Next Actions

1. Send this report and hash manifest to vendor security contact.
2. Request Steam-side manifest, actor, and cache evidence fields above.
3. Archive completed evidence package under incident tracking ticket.
