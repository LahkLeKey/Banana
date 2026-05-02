# Banana Backlog — 3-Month Sprint Plan (May–July 2026)

**Generated**: 2026-05-01
**Backlog scored**: #052–#101 (50 stubs; #053 already shipped)
**Scoring dimensions**: Value Gain (V), Risk Reduction (R), Dependency Unlock (D), Effort inverse (E). Max = 20.

---

## Scoring Table

| # | Stub | V | R | D | E | **Total** | Blocker? |
|---|------|---|---|---|---|-----------|----------|
| 052 | frontend-routing-architecture | 4 | 2 | 3 | 3 | **12** | — |
| 054 | frontend-streaming-verdict-and-chat | 5 | 2 | 1 | 3 | **11** | — |
| 055 | frontend-pwa-offline-shell | 3 | 2 | 1 | 3 | **9** | — |
| 056 | shared-ui-storybook-and-docs | 3 | 1 | 1 | 4 | **9** | — |
| 057 | frontend-accessibility-wcag22-gate | 4 | 3 | 1 | 3 | **11** | — |
| 058 | frontend-i18n-localization-scaffold | 3 | 1 | 1 | 3 | **8** | — |
| 059 | frontend-theming-and-dark-mode | 3 | 1 | 1 | 4 | **9** | — |
| 060 | frontend-e2e-playwright-harness | 4 | 3 | 1 | 3 | **11** | needs 052 |
| 061 | frontend-observability-and-vitals | 4 | 4 | 5 | 3 | **16** | — |
| 062 | frontend-performance-budget-gate | 3 | 3 | 1 | 4 | **11** | needs 061 |
| 063 | frontend-auth-and-roles | 5 | 5 | 4 | 2 | **16** | — |
| 064 | frontend-operator-dashboard | 4 | 2 | 2 | 2 | **10** | needs 063 |
| 065 | frontend-neuro-trace-viewer | 4 | 2 | 1 | 2 | **9** | needs 053✅, 061 |
| 066 | mobile-parity-sprint | 4 | 2 | 1 | 2 | **9** | needs 052 |
| 067 | api-openapi-and-typed-clients | 4 | 2 | 1 | 4 | **11** | — |
| 068 | api-rate-limiting-and-abuse-protection | 4 | 5 | 1 | 3 | **13** | needs 063 |
| 069 | api-audit-log-surface | 4 | 4 | 2 | 3 | **13** | needs 063 |
| 070 | api-background-jobs-runner | 4 | 3 | 3 | 3 | **13** | — |
| 071 | api-webhook-delivery-system | 4 | 2 | 1 | 3 | **10** | needs 070 |
| 072 | native-abi-versioning-contract | 3 | 5 | 2 | 3 | **13** | — |
| 073 | native-sanitizer-ci-lane | 2 | 5 | 2 | 3 | **12** | — |
| 074 | native-cross-platform-build-matrix | 3 | 3 | 2 | 3 | **11** | needs 072 |
| 075 | native-pgo-and-perf-baseline | 3 | 2 | 1 | 3 | **9** | needs 073 |
| 076 | training-cross-model-joint-pipeline | 4 | 2 | 1 | 2 | **9** | needs 050✅, 079 |
| 077 | training-active-learning-loop | 4 | 2 | 1 | 2 | **9** | needs 050✅, 064 |
| 078 | training-drift-detection-on-inference | 4 | 4 | 1 | 3 | **12** | needs 061 |
| 079 | training-model-registry-signed-releases | 4 | 5 | 5 | 3 | **17** | — |
| 080 | training-ab-harness-live-comparison | 4 | 2 | 1 | 3 | **10** | needs 079, 061 |
| 081 | infra-helm-and-k8s-manifests | 3 | 3 | 1 | 2 | **9** | — |
| 082 | infra-secrets-management-vault | 4 | 5 | 1 | 2 | **12** | — |
| 083 | infra-disaster-recovery-and-backups | 3 | 5 | 1 | 2 | **11** | needs 079 |
| 084 | infra-multi-arch-container-images | 3 | 2 | 2 | 3 | **10** | needs 074 |
| 085 | quality-mutation-testing-baseline | 2 | 3 | 1 | 3 | **9** | — |
| 086 | supply-chain-sbom-and-signing | 3 | 4 | 2 | 3 | **12** | needs 084 |
| 087 | devex-monorepo-task-graph | 3 | 2 | 1 | 4 | **10** | — |
| 088 | devex-devcontainer-and-codespaces | 3 | 2 | 1 | 4 | **10** | — |
| 089 | devex-pre-commit-hooks-baseline | 3 | 3 | 1 | 5 | **12** | — |
| 090 | devex-feature-flag-platform | 4 | 3 | 1 | 3 | **11** | — |
| 091 | devex-error-tracking-sentry | 4 | 3 | 1 | 4 | **12** | needs 061 |
| 092 | perf-rum-and-core-web-vitals | 3 | 2 | 1 | 4 | **10** | needs 061 |
| 093 | perf-synthetic-monitoring-blackbox | 3 | 3 | 1 | 3 | **10** | needs 063 |
| 094 | perf-load-testing-baseline | 3 | 3 | 1 | 3 | **10** | — |
| 095 | data-postgres-migration-discipline | 4 | 5 | 1 | 4 | **14** | — |
| 096 | data-pii-classification-and-redaction | 4 | 5 | 1 | 2 | **12** | needs 069 |
| 097 | data-event-sourcing-for-training | 4 | 3 | 1 | 2 | **10** | needs 070, 079 |
| 098 | security-threat-model-and-stride | 3 | 5 | 2 | 4 | **14** | — |
| 099 | security-third-party-pentest-readiness | 4 | 4 | 1 | 2 | **11** | needs 098, 086 |
| 100 | security-csp-and-headers-hardening | 3 | 5 | 1 | 4 | **13** | — |
| 101 | security-dependency-update-bot | 3 | 5 | 1 | 5 | **14** | — |

---

## Top-20 Ranked

Ties broken by: (a) fewer blockers, (b) higher dependency unlock count, (c) lower wave number.

| Rank | # | Stub | Total | Status |
|------|---|------|-------|--------|
| 1 | **079** | training-model-registry-signed-releases | 17 | Unblocked |
| 2 | **061** | frontend-observability-and-vitals | 16 | Unblocked — unlocks 6 items |
| 3 | **063** | frontend-auth-and-roles | 16 | Unblocked — unlocks 4 items |
| 4 | **101** | security-dependency-update-bot | 14 | Unblocked — quick win |
| 5 | **095** | data-postgres-migration-discipline | 14 | Unblocked |
| 6 | **098** | security-threat-model-and-stride | 14 | Unblocked — unlocks 099 |
| 7 | **070** | api-background-jobs-runner | 13 | Unblocked — unlocks 071, 097 |
| 8 | **072** | native-abi-versioning-contract | 13 | Unblocked — unlocks native chain |
| 9 | **100** | security-csp-and-headers-hardening | 13 | Unblocked |
| 10 | **068** | api-rate-limiting-and-abuse-protection | 13 | Blocked by 063 |
| 11 | **069** | api-audit-log-surface | 13 | Blocked by 063 |
| 12 | **052** | frontend-routing-architecture | 12 | Unblocked — unlocks 060, 066 |
| 13 | **089** | devex-pre-commit-hooks-baseline | 12 | Unblocked — quick win |
| 14 | **073** | native-sanitizer-ci-lane | 12 | Unblocked |
| 15 | **082** | infra-secrets-management-vault | 12 | Unblocked (pull-forward) |
| 16 | **078** | training-drift-detection-on-inference | 12 | Blocked by 061 |
| 17 | **091** | devex-error-tracking-sentry | 12 | Blocked by 061 |
| 18 | **086** | supply-chain-sbom-and-signing | 12 | Blocked by 084 |
| 19 | **096** | data-pii-classification-and-redaction | 12 | Blocked by 069 |
| 20 | **054** | frontend-streaming-verdict-and-chat | 11 | Unblocked |

---

## 3-Month Sprint Sequence

### Sprint 1 — Month 1 (May 2026): "Land the Foundations That Unblock Everything"

Ship the highest-unlock unblocked items across all domains simultaneously.

| # | Stub | Rationale |
|---|------|-----------|
| **101** | security-dependency-update-bot | Highest ratio quick win; ships day 1 |
| **089** | devex-pre-commit-hooks-baseline | Quality gate; unblocked and fast |
| **061** | frontend-observability-and-vitals | Unblocks 6 downstream stubs |
| **079** | training-model-registry-signed-releases | Unblocks 5 downstream stubs; supply chain integrity |
| **072** | native-abi-versioning-contract | Unblocks native → infra → supply chain chain |
| **052** | frontend-routing-architecture | Unblocks 060 (E2E) and 066 (mobile) |

**Exit criteria:** Observability pipeline live; model registry signed; ABI contract versioned in CI; pre-commit hooks passing; routing architecture merged.

---

### Sprint 2 — Month 2 (June 2026): "Security Core, Data Foundation, API Depth"

Activate auth, harden the data layer, and open the API surface.

| # | Stub | Rationale |
|---|------|-----------|
| **063** | frontend-auth-and-roles | Unblocks 068, 069, 093; critical security |
| **095** | data-postgres-migration-discipline | High risk reduction; unblocked |
| **098** | security-threat-model-and-stride | Unblocks pentest readiness; unblocked |
| **070** | api-background-jobs-runner | Unblocks 071, 097; unblocked |
| **100** | security-csp-and-headers-hardening | Unblocked security hardening |
| **073** | native-sanitizer-ci-lane | Unblocks 075; CI risk reduction |

**Exit criteria:** Auth roles live; migration CI gate enforced; threat model published; background job infra live; CSP headers enforced.

---

### Sprint 3 — Month 3 (July 2026): "Depth Layers — Compliance, Monitoring, Cross-Platform"

Harvest sprint 1/2 unlocks.

| # | Stub | Rationale |
|---|------|-----------|
| **068** | api-rate-limiting-and-abuse-protection | Unlocked by 063 |
| **069** | api-audit-log-surface | Unlocked by 063; unlocks 096 |
| **078** | training-drift-detection-on-inference | Unlocked by 061 |
| **082** | infra-secrets-management-vault | No blocker; risk=5; pull forward |
| **074** | native-cross-platform-build-matrix | Unlocked by 072; unlocks 084 |
| **054** | frontend-streaming-verdict-and-chat | Highest user-facing value; no blocker |

**Exit criteria:** Rate limiting on all API endpoints; audit log queryable; drift alerts wired; secrets in Vault; ARM64 + x86-64 native builds green.

---

## Cross-Cutting Notes

- **#063 auth** — sprint-2 bottleneck for #068, #069, #093, #096. Effort score 2/5 — resist scope creep.
- **#061 observability** — single largest unlocker (6 items). Cascade risk: if it slips, #062/#065/#078/#091/#092 all slip.
- **#079 model registry** — unlocks #076/#080/#083/#086/#097. #076 and #077 also need #050 (already shipped ✅).
- **Supply-chain chain depth**: `072 → 074 → 084 → 086 → 099` = 5 hops. Earliest #099 (pentest) lands: month 5.
- **#082 Vault pull-forward** — labeled fourth-wave but no blockers and risk=5. Sprint 3 is correct; don't wait.
- **#096 PII redaction** — compliance exposure, R=5. Depends on #069 (sprint 3). First item to schedule in sprint 4.
- **Wave labels vs. dependency graph** — #073 is labeled second-wave but unblocked from day one. Use the dependency graph as the scheduling authority, not the wave label.

---

## Stubs Deferred to Month 4+

#054 already in sprint 3. Remaining deferred: #055, #056, #057, #058, #059, #060 (needs 052), #062 (needs 061), #064 (needs 063), #065 (needs 061), #066 (needs 052), #067, #071 (needs 070), #075 (needs 073), #076 (needs 079), #077 (needs 064), #080 (needs 079+061), #081, #083 (needs 079), #084 (needs 074), #085, #086 (needs 084), #087, #088, #090, #091 (needs 061), #092 (needs 061), #093 (needs 063), #094, #096 (needs 069), #097 (needs 070+079), #099 (needs 098+086).
