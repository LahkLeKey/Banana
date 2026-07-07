# Client Applications Context

## Purpose

Define client-facing behavior, interaction flows, and integration with backend/runtime surfaces.

## In scope

- UI behavior and interaction patterns
- Client-side state and shell/runtime integration
- Consumption of API and shared package contracts

## Out of scope

- Server-side domain/persistence internals
- Native engine algorithm internals

## Ubiquitous language

- Flow: User-visible sequence of interactions and state transitions
- Client contract: Assumption the UI makes about API/shared package behavior
- Gameplay shell: Authenticated, page-based client area with shared navigation across gameplay and account pages
- Gameplay home: Default landing experience inside the gameplay shell after authentication
- Gameplay route slice: Focused page in the gameplay shell that exposes one gameplay concern while reusing shared client capabilities
- Notebook gameplay surface: Visualization-heavy gameplay surface that should be embedded into canonical gameplay routes when it adds player value
- World map slice: Canonical gameplay route where dense world-state visualizations are hosted
- Primary gameplay navigation: Stable-route-only top-level navigation for canonical gameplay pages
- Character-gated gameplay routes: Gameplay slices that require an active character selection before entry
- Home orientation state: Authenticated home experience that stays accessible before character selection and guides the player to the next required step
- Route gate screen: Inline route-level explanation shown when a gated gameplay slice is requested without required prerequisites
- Character selection adapter: Client-side seam that mediates character selection read/write so storage backend can change without route/page rewrites
- Locked navigation destination: Stable primary-nav route that remains visible before prerequisites are met and communicates blocked status
- Active operator selection: The single currently selected character used as gameplay context for gated slices
- Operator switch policy: Rule that character changes are initiated from the Characters slice rather than implicit global controls
- Return-to-intended-route flow: Login flow that restores the originally requested protected route after authentication
- Public login gate: Login route that remains public and only serves as an entry point for unauthenticated users
- Identity handoff flow: Redirect-and-return authentication sequence between client and identity authority
- Auth authority mode: Client runtime mode that determines which identity authority login flow is active
- Bearer continuity: Client behavior that carries the active bearer token across protected API calls until expiry or sign-out
- OIDC PKCE flow: Authorization Code flow with Proof Key for Code Exchange used by a public browser client without a client secret
- Ephemeral token storage: In-memory bearer token handling that does not persist tokens to localStorage or sessionStorage

## Core invariants

- User-visible behavior should be consistent across supported runtime modes
- Client assumptions about API contracts must remain explicit and testable
- Gameplay pages are delivered through a shared authenticated gameplay shell rather than route aliases to a single prototype surface
- Gameplay home is the default post-login landing experience
- Existing notebook visualizations are folded into canonical gameplay routes when they support the active gameplay slice
- World map is the first canonical route that absorbs the densest notebook-style visualization surface
- Gameplay home remains lighter-weight and emphasizes orientation plus navigation into gameplay slices
- Primary gameplay navigation exposes only stable canonical routes: Home, Characters, World Map, and Profile
- In-progress slices are discoverable from secondary home-page affordances rather than primary navigation
- World map and other gated gameplay slices require an active selected character and redirect to /characters when missing
- Home remains accessible without character selection and provides a prominent route to /characters
- Gated route access uses inline route gate screens with clear prerequisite messaging and a primary transition to /characters
- Character selection persistence remains local in this branch, but all access goes through the character selection adapter seam
- Character selection model is single-active-operator for this branch; adapter shape remains extensible for future multi-slot account-backed evolution
- Operator switching is performed from the Characters route; primary nav may display the active operator but does not mutate selection directly
- Authentication preserves intended destination before applying route gates so login returns the user to the protected route they tried to open
- Signed-in users are redirected away from /login immediately so the login route stays a one-way public gate
- Stable primary-nav destinations remain visible even when gated; blocked destinations surface a locked state and route to the inline gate screen when requested
- Login flow must map to the active identity authority mode and avoid parallel authority assumptions
- Keycloak is the single authentication authority for active client login and bearer continuity
- Phase-1 protected API access relies on bearer continuity plus API-side token authority checks
- Profile is part of the authenticated gameplay shell rather than a separate application mode
- Keycloak browser login uses OIDC Authorization Code + PKCE and does not require a frontend client secret
- Phase-1 bearer tokens remain in ephemeral memory and are cleared on reload or sign-out
- In Keycloak mode, client startup blocks and surfaces configuration errors rather than silently degrading protected flows

## Key seams

- API service request/response boundaries
- Shared UI and resilience package boundaries

## Open questions

- Steam login paths are retired and out of scope for active client authentication
- Keycloak deployment topology is one Fly app per environment to isolate login incidents
- Phase-1 OIDC mode is Authorization Code + PKCE for the React client
- Phase-1 token storage mode is in-memory only
- Keycloak rollout mode defaults to enabled and hard-fails startup on invalid configuration
