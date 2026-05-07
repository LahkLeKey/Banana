# Spike Report — Frontend Robustness Uplift

**Feature**: 051-frontend-robustness-spike
**Generated**: 2026-05-01
**Author**: spike (no code shipped)

## 1. Current frontend surface (inventory)

### React (`src/typescript/react`)
- Single page: [`src/typescript/react/src/App.tsx`](../../../src/typescript/react/src/App.tsx) (~600+ LOC, all UX in one component tree).
- shadcn primitives wired (feature 048): Alert, Button, Card, Input, Textarea under `components/ui/`.
- Bespoke `ErrorBoundary` only.
- No client routing; the React app is effectively one screen with conditional regions for chat, ripeness, ensemble verdict, escalation.
- No state library — local React state + bespoke `lib/resilience-bootstrap.ts` queue.
- API client: handwritten `fetch` wrappers in `src/lib/api.ts`. No request cache, no streaming.
- Tests: Bun + happy-dom + Testing Library. ~10 component tests; no e2e.

### Shared UI (`src/typescript/shared/ui`)
- 5 components: BananaBadge, ChatMessageBubble, ErrorText, EscalationPanel, RetryButton, RipenessLabel.
- `tokens/` directory with JSON tokens + a generator script that emits Tailwind tokens.
- No Storybook, no visual regression harness, no public component API doc.

### Electron (`src/typescript/electron`)
- Main/preload/ipc/notifications/storage/tray + `smoke.js` harness.
- Renders the same React build inside a BrowserWindow; no desktop-specific UX (no command palette, no menubar polish, no native notifications surface).

### React-Native (`src/typescript/react-native`)
- 3 screens: Capture, History, Verdict.
- Thin `lib/` (api.ts, copy.ts, resilience-bootstrap.ts).
- No share-extension wiring beyond `share-handler.ts`; no offline cache, no push notifications, no biometric auth.

### Resilience layer
- Retry policy + offline queue (resilience-bootstrap) is solid but only wired to the ensemble submit path. Not used for chat, ripeness, or any future streaming endpoint.

## 2. Gap analysis vs. backend depth

The backend surface in this repo is large: ASP.NET pipeline + native interop, Fastify+Prisma TS API, three trainers with the new neuroscience-inspired self-training cycle (feature 050), validation governance (feature 047 parity gates), AI contract guards, compose-orchestrated runtimes, workflow recovery (feature 049). Customer-visible value is gated by a single React page — the depth gap is severe.

## 3. Nine uplift themes (ranked by leverage)

### 3.1 Application architecture
**Problem**: Single-page App.tsx mixes 5 concerns (chat, ensemble, ripeness, escalation, history). Adding any new feature increases coupling.
**Direction**: File-based routing (TanStack Router or React Router 7), code-splitting per route, layout primitives in shared/ui.
**Sources**: TanStack Router docs (https://tanstack.com/router); React Router v7 framework mode; Vercel "Why split routes" guide.
**Maps to**: 052 routing.

### 3.2 Typed client state
**Problem**: All async state is local React + ad-hoc fetch. No cache, no background refresh, no optimistic updates.
**Direction**: TanStack Query for server state + Zustand (or Jotai) for client state. Strict TS types from API openapi.
**Sources**: TanStack Query v5 docs; Zustand patterns; "Server state vs client state" (Tanner Linsley).
**Maps to**: 053 state baseline.

### 3.3 Real-time UX
**Problem**: Verdict + chat both block on a full HTTP roundtrip. The neuro layer (feature 050) emits trace events that have nowhere to surface in the UI.
**Direction**: Server-Sent Events for verdict streaming + chat tokens. WebSocket (or SSE multiplex) for training-run progress.
**Sources**: WHATWG EventSource spec; HTMX SSE swap patterns; OpenAI streaming chat patterns.
**Maps to**: 054 streaming, 065 neuro-trace viewer.

### 3.4 Offline & resilience
**Problem**: Resilience queue exists but the app shell itself is non-functional offline. Mobile users on flaky networks get blank pages.
**Direction**: PWA install + service worker app shell (Workbox); use existing resilience queue as the durable submit layer.
**Sources**: web.dev PWA series; Workbox v7; Apple "Designing Web Apps for the Home Screen" notes.
**Maps to**: 055 PWA.

### 3.5 Design system depth
**Problem**: 5 shared components, no docs, no visual regression. Theming = "default Tailwind".
**Direction**: Storybook 8 + Chromatic-style snapshot testing in CI; CSS variables for theming.
**Sources**: Storybook 8 release notes; shadcn theming guide; Radix Themes color system.
**Maps to**: 056 storybook, 059 theming.

### 3.6 Quality gates (a11y / perf / observability)
**Problem**: No a11y CI, no performance budget, no production telemetry. We ship blind.
**Direction**: axe-core + @testing-library/jest-dom in unit; Playwright + axe in e2e; Lighthouse CI for perf budget; Web Vitals + OpenTelemetry browser SDK for production telemetry.
**Sources**: WCAG 2.2 AA reference; web.dev Core Web Vitals; OpenTelemetry JS SDK browser autoinstrumentation.
**Maps to**: 057 a11y, 061 observability, 062 perf budget, 060 e2e.

### 3.7 Internationalization
**Problem**: All copy is hardcoded English in `lib/copy.ts`. No locale negotiation.
**Direction**: i18next or FormatJS; integrate with shared/ui token pipeline so locale-aware components are first-class.
**Sources**: ICU MessageFormat; FormatJS docs; i18next React adapter.
**Maps to**: 058 i18n.

### 3.8 Auth & roles
**Problem**: There is no client authentication surface. Operator-only views (training metrics, neuro-trace, audit) are blocked from existing because there's nothing to gate them on.
**Direction**: OIDC PKCE flow against an external IdP (or device-bound local accounts for the desktop channel); role-aware route guards.
**Sources**: OAuth 2.1 BCP, IETF OAuth 2.0 for Browser-Based Apps draft; Auth.js v5 patterns.
**Maps to**: 063 auth, 064 operator dashboard.

### 3.9 Mobile parity
**Problem**: 3 RN screens vs the API's full surface. No share-target, no offline cache, no push notifications.
**Direction**: Share-extension polish, expo-secure-store for credentials, FlashList for history, push via Expo Notifications.
**Sources**: Expo SDK 51 docs; Apple Share Extension HIG; Android intent filter docs.
**Maps to**: 066 mobile parity.

## 4. Dependency map

```
052 routing  ┐
053 state     ├─► 054 streaming ─► 065 neuro-trace viewer
055 PWA       │                  ╲
056 storybook ┤                   └─► 064 operator dashboard
057 a11y      │       ╱─ 063 auth ─┘
058 i18n      │      ╱
059 theming   │     ╱
060 e2e       ┴─► 061 observability ─► 062 perf budget
066 mobile parity (parallel, depends on 053 + 057)
```

First wave (no prerequisites beyond current main): 052, 053, 056.
Second wave (depends on first): 054, 055.
Third wave (cross-cutting): 057, 058, 059, 060.
Fourth wave (depends on observability + auth): 061, 062, 063.
Fifth wave (depends on auth/observability): 064, 065, 066.

## 5. Pure-frontend vs cross-layer features

**Pure frontend (no API/native changes required)**: 052, 053, 055, 056, 057, 058, 059, 060, 062, 066.

**Cross-layer (requires API or native coordination)**:
- 054 streaming — needs SSE or WebSocket endpoints in Fastify + ASP.NET.
- 061 observability — needs trace ingestion endpoint or OTLP collector.
- 063 auth — needs IdP integration on API.
- 064 operator dashboard — needs metrics-read API endpoints.
- 065 neuro-trace viewer — needs read-only `artifacts/training/*/local/neuro-trace.json` exposure.

## 6. Non-goals for this spike

- No code edits to React/Electron/RN/shared in this PR.
- No new API endpoints in this PR (they are scoped into 054/061/063/064/065 specs).
- No commitment to a specific routing/state library — each follow-up spec will run its own ADR.

## 7. References

1. WCAG 2.2 — https://www.w3.org/TR/WCAG22/
2. TanStack Router — https://tanstack.com/router
3. TanStack Query v5 — https://tanstack.com/query/latest
4. WHATWG EventSource — https://html.spec.whatwg.org/multipage/server-sent-events.html
5. Workbox v7 — https://developer.chrome.com/docs/workbox
6. Storybook 8 — https://storybook.js.org/blog/storybook-8/
7. axe-core — https://github.com/dequelabs/axe-core
8. Lighthouse CI — https://github.com/GoogleChrome/lighthouse-ci
9. Web Vitals — https://web.dev/vitals/
10. OpenTelemetry JS Browser SDK — https://opentelemetry.io/docs/instrumentation/js/getting-started/browser/
11. FormatJS — https://formatjs.io/
12. OAuth 2.0 for Browser-Based Apps — https://datatracker.ietf.org/doc/html/draft-ietf-oauth-browser-based-apps
13. Expo SDK 51 — https://docs.expo.dev/
14. shadcn/ui theming — https://ui.shadcn.com/docs/theming
15. Radix Themes color system — https://www.radix-ui.com/themes/docs/theme/color
