# Feature Specification: Steam Launch Gating

**Feature Branch**: `[008-steam-launch-gating]`

**Created**: 2026-05-24

**Status**: Draft

**Input**: User description: "Native runtime launch must be hard-gated by Steam identity/account verification against production account status, with practical development-mode guardrails."

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Block Unverified Runtime Launch (Priority: P1)

As a player launching the native runtime, I need launch access to be allowed only when my Steam identity is validated and my linked production account status is valid, so blocked or unlinked accounts cannot enter gameplay.

**Why this priority**: This is the core security and access-control outcome. If this is not enforced, the feature intent fails.

**Independent Test**: Can be fully tested by attempting launch with valid, invalid, suspended, and unlinked account states and verifying only valid state launches gameplay.

**Acceptance Scenarios**:

1. **Given** a player has a valid Steam identity and verified linked production account in good standing, **When** they start runtime launch, **Then** launch proceeds to gameplay without account-gating errors.
2. **Given** a player has a Steam identity but no verified linked production account, **When** they start runtime launch, **Then** gameplay launch is blocked and a clear remediation path is shown.
3. **Given** a player account is suspended or otherwise not in good standing, **When** they start runtime launch, **Then** gameplay launch is blocked and the block reason is shown.

---

### User Story 2 - Complete Registration or Linking Through Web Flow (Priority: P2)

As a player who fails launch verification, I need a clear path to register and link my account via web, then retry runtime launch, so I can recover access without support intervention.

**Why this priority**: Recovery and conversion are essential to reduce abandonment and support load once gating is enforced.

**Independent Test**: Can be tested by failing launch due to unlinked status, completing web registration/linking, and confirming launch succeeds on retry.

**Acceptance Scenarios**:

1. **Given** launch is blocked due to missing linkage, **When** player follows the provided web registration/linking path and completes required steps, **Then** a subsequent launch attempt can pass verification.
2. **Given** launch is blocked and network access to web flow is unavailable, **When** player requests remediation, **Then** the system presents retry guidance and fallback messaging without bypassing launch gate.

---

### User Story 3 - Practical Development Workflow Guardrails (Priority: P3)

As a developer, I need explicit environment-mode behavior so local build and run workflows remain practical when Steam context is unavailable, while deployed production launch remains strictly gated.

**Why this priority**: Development throughput must remain viable, but production enforcement cannot be weakened.

**Independent Test**: Can be tested by running launch in allowed development modes without Steam path and confirming it is denied in production-mode launch contexts unless verification succeeds.

**Acceptance Scenarios**:

1. **Given** runtime is started in an approved development mode, **When** Steam verification context is absent, **Then** launch behavior follows explicit development policy and records that production gating was not used.
2. **Given** runtime is started in production launch context, **When** verification cannot be completed, **Then** gameplay launch is blocked with no development bypass path.

---

### Edge Cases

- What happens when verification services are temporarily unavailable at launch time?
- How does the system handle stale or expired identity/session evidence presented at launch?
- What happens when cached status conflicts with a newer authoritative account state (for example, newly suspended account)?
- How does launch gating behave when the player is offline and no recent verifiable status exists?
- How does the system handle repeated rapid retry attempts after consecutive verification failures?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST require launch-time verification of Steam identity and linked production account status before allowing gameplay in production launch contexts.
- **FR-002**: System MUST deny gameplay launch when Steam identity cannot be validated, account linkage is missing, account status is suspended/restricted, or verification status is indeterminate.
- **FR-003**: System MUST provide explicit block reasons that distinguish at minimum: unlinked account, suspended/restricted account, stale/expired verification evidence, offline/no verification path, and upstream verification service unavailability.
- **FR-004**: System MUST provide a remediation path for blocked users that includes web registration and account-linking instructions when the failure reason is remediable.
- **FR-005**: System MUST require successful verification on a subsequent launch attempt after remediation, rather than granting automatic gameplay access solely from prior failure context.
- **FR-006**: System MUST define and enforce environment-mode policy that differentiates approved development workflows from production launch workflows.
- **FR-007**: System MUST ensure approved development-mode behavior cannot be used as a bypass in deployed production launch channels.
- **FR-008**: System MUST cover native engine launch boundaries, ABI-facing launch contracts, API verification decisions, and web account-linking interplay under one coherent launch-gating policy.
- **FR-009**: System MUST treat verification as time-sensitive and reject stale or expired verification evidence according to policy.
- **FR-010**: System MUST define offline behavior such that production launch remains blocked when authoritative verification cannot be established.
- **FR-011**: System MUST support user-initiated retry after transient failures and return updated block/allow decisions based on latest verification outcome.
- **FR-012**: System MUST emit auditable launch decision events for allow/deny outcomes, including reason codes and environment mode classification.

### Key Entities *(include if feature involves data)*

- **Steam Identity Assertion**: Evidence that runtime launch request is associated with a specific Steam user identity at launch time.
- **Linked Production Account Status**: Authoritative account-state record describing whether the Steam identity is linked and in good standing for gameplay access.
- **Launch Verification Decision**: Final allow/deny decision with reason code, decision timestamp, and mode context.
- **Verification Session Context**: Launch attempt context containing verification freshness, retry metadata, and remediation state.
- **Environment Mode Policy**: Rule set classifying launch contexts (for example development vs production) and defining required verification strictness.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 100% of production launch attempts with missing, invalid, suspended, or indeterminate account status are blocked from entering gameplay.
- **SC-002**: At least 95% of blocked users can identify the specific block reason and the next remediation step from the presented message without contacting support.
- **SC-003**: At least 90% of users who complete required web registration/linking can successfully pass launch verification on their next launch attempt.
- **SC-004**: In production launch context, 0 verified incidents occur where gameplay starts without a valid Steam-linked account in good standing.
- **SC-005**: For transient verification failures (for example service unavailable), at least 95% of successful retries reflect updated status within one retry attempt after service recovery.

## Assumptions

- Steam identity and production account status each have an authoritative source of truth available through existing business systems.
- Web registration and account-linking experience already exists or can be presented as an external remediation surface.
- Development teams require an explicitly documented non-production mode for local workflows where Steam launch path is unavailable.
- Production launch channels are distinguishable from development workflows through environment-mode classification.
- No gameplay systems are redesigned by this feature; only launch-access gating behavior and related user messaging are in scope.

## Constitution Alignment *(mandatory)*

- **Disclosure Integrity**: Player-facing access claims must accurately state that gameplay launch requires a valid Steam-linked account in good standing for production channels. Any user-facing launch error text must align with this requirement and remediation guidance.
- **Cross-Domain Contracts**: Affected contracts include native runtime launch checkpoints, ABI launch decision boundary, API account verification decision contract, and web registration/linking remediation contract.
- **Quality Gates**: Validation must cover allow-path and deny-path outcomes across: valid account, unlinked account, suspended account, stale verification, offline verification, and verification service outage; plus environment-mode policy checks for development vs production contexts.
- **Delivery Evidence**: Evidence includes reproducible launch-attempt logs for each failure mode and success mode, policy conformance results for environment-mode behavior, and user-visible block/retry/remediation message captures.

## Storefront & Release Claims *(required when public Steam copy is affected)*

- **Storefront Copy Contract**: If public product-access text references account prerequisites, it must explicitly include Steam-linked account verification requirement and good-standing condition.
- **AI Disclosure**: No new AI disclosure claim is introduced by this feature.
- **Controller Support**: Controller support claims are unchanged by this feature and remain validated by existing controller verification gates.
- **System Requirements**: Baseline hardware/software claims are unchanged; account-access prerequisite claims are updated only if public copy currently omits launch-gating requirement.
- **Store Assets**: No new media assets are required solely for this feature unless release communications mandate updated support screenshots or storefront text captures.
