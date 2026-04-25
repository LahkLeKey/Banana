# Feature Specification: Chatbot Platform Interop

**Feature Branch**: `016-chatbot-platform-interop`  
**Created**: 2026-04-25  
**Status**: Draft  
**Parent**: [`007-aspnet-pipeline`](../007-aspnet-pipeline/spec.md), [`008-typescript-api`](../008-typescript-api/spec.md), [`009-react-shared-ui`](../009-react-shared-ui/spec.md), [`010-electron-desktop`](../010-electron-desktop/spec.md), [`011-react-native-mobile`](../011-react-native-mobile/spec.md)

## Summary

Define an API-first chatbot interaction contract that supports consistent conversations across Banana platforms (web portal, Electron desktop portal, and mobile portal). The API contract must be canonical and platform surfaces must reuse shared interaction primitives so a user can continue a conversation between platforms without behavior drift.

## User Scenarios & Testing

### User Story 1 — API-first conversation lifecycle (Priority: P1)

As a platform client, I can create a chat session, send messages, and receive assistant responses through a single canonical API contract.

**Independent Test**: Create session, post message, fetch responses, and verify response envelope fields remain stable.

**Acceptance Scenarios**:

1. **Given** a new user conversation, **When** the client creates a session, **Then** API returns a stable session identifier and metadata.
2. **Given** an existing session, **When** the client posts a user message, **Then** API returns message status and assistant response payload in the canonical format.

### User Story 2 — Cross-platform continuity (Priority: P1)

As an end user, I can start chatting on one portal and continue on another while preserving message history and context.

**Independent Test**: Start conversation in one client, continue in another client using same session, and verify timeline parity.

**Acceptance Scenarios**:

1. **Given** a valid session with existing history, **When** the user opens another portal, **Then** conversation history is replayed in deterministic order.
2. **Given** platform-specific UI limitations, **When** rendering assistant content, **Then** unsupported rich elements degrade gracefully without losing message meaning.

### User Story 3 — Predictable failure handling (Priority: P2)

As a platform client, I receive typed errors and retry guidance when chat requests fail due to validation, upstream unavailability, or policy gating.

**Independent Test**: Force representative failures and verify API and portal error contracts map consistently.

### User Story 4 — Shared portal interaction primitives (Priority: P2)

As a frontend maintainer, I can implement chatbot UI behavior using shared primitives so web, desktop, and mobile remain consistent while allowing platform-specific rendering.

**Independent Test**: Shared message model and interaction states are consumed by all portal clients with bounded platform-specific adapters.

## Edge Cases

- Session resume with missing or expired session identifiers.
- Duplicate message submission due to client retries.
- Partial response streaming interruptions and recovery.
- Cross-platform rendering mismatch for rich assistant payload blocks.

## Requirements

### Functional Requirements

- **FR-001**: A canonical chatbot API contract MUST define session create/read/write flows and message exchange envelopes.
- **FR-002**: API contract MUST define idempotent message submission semantics to prevent duplicate assistant actions.
- **FR-003**: API contract MUST define typed error categories and retry guidance fields.
- **FR-004**: API contract MUST support cross-platform conversation continuity using stable session and message identifiers.
- **FR-005**: Portal clients (web, desktop, mobile) MUST consume the canonical API contract without per-platform payload forks.
- **FR-006**: Shared UI/domain primitives MUST define message timeline ordering, assistant typing state, and delivery status transitions.
- **FR-007**: Portal clients MUST support graceful degradation for unsupported rich assistant payload elements.
- **FR-008**: Platform integration MUST preserve existing base URL/runtime contracts (`VITE_BANANA_API_BASE_URL`, desktop bridge configuration, and mobile runtime API configuration).
- **FR-009**: Observability fields MUST allow correlation of user requests, assistant responses, and platform client events.
- **FR-010**: Security and privacy constraints MUST define per-message metadata boundaries and retention assumptions.

### Hard contracts to preserve

- Existing API ownership boundaries between ASP.NET and TypeScript API domains.
- Shared UI contract in `@banana/ui` for cross-portal primitives.
- Existing runtime base URL and environment contract conventions.

### Key Entities

- **Chat Session**: stable conversation identifier and metadata for continuity.
- **Chat Message**: user/assistant/system message with lifecycle status and ordering metadata.
- **Assistant Response Envelope**: canonical payload containing content blocks, citations/tool events (if any), and policy metadata.
- **Portal Adapter**: platform-specific bridge that maps canonical chat contract to platform UI runtime.

## Success Criteria

- **SC-001**: A single chatbot API contract is documented and consumable by all portals.
- **SC-002**: Cross-platform continuation succeeds for primary conversation flows without history divergence.
- **SC-003**: Chat failure classes map to consistent portal behavior and user-facing guidance.
- **SC-004**: Shared chat interaction primitives are reused across web, desktop, and mobile with no contract forks.
- **SC-005**: Chatbot API and portal contract behavior can be validated through automated scenarios for session lifecycle and message exchange.

## Assumptions

- This feature slice specifies interaction contracts and implementation boundaries first; full chatbot provider orchestration may be delivered in subsequent slices.
- Authentication and authorization primitives already exist or are provided by adjacent API platform slices.
- Existing portal architecture can add shared chat surfaces without replacing current app navigation shells.
