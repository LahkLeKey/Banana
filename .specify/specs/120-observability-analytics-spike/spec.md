# Feature Specification: Observability and Analytics Spike (120)

**Feature Branch**: `feature/120-observability-analytics-spike`
**Created**: 2026-05-02
**Status**: Spike — research and go/no-go only
**Input**: `banana.engineer` is launching publicly. Identify the minimum observability stack (real-user analytics, error tracking, uptime monitoring) compatible with the Vercel + React deployment and free/low-cost tier constraints.

## In Scope *(mandatory)*

- Evaluate Vercel Analytics (free tier) for page-view and performance tracking in the React app.
- Evaluate Vercel Speed Insights for real-user Core Web Vitals reporting.
- Evaluate Sentry (free tier) for browser error tracking with source-map upload in the Vercel build.
- Evaluate a lightweight uptime monitor (Better Uptime, UptimeRobot) for `banana.engineer` and `api.banana.engineer`.
- Identify the React SDK integration points for each tool (no backend changes in scope).
- Document findings, pricing, privacy implications, and a concrete path to specs 137–140.

## Out of Scope *(mandatory)*

- Backend APM (Datadog, New Relic) for the API.
- Session replay tools (FullStory, LogRocket) — privacy risk outside this spike.
- OpenTelemetry or custom trace pipeline.
- Mobile or Electron observability.

## User Scenarios & Testing *(mandatory)*

### User Story 1 — On-call engineer is paged when banana.engineer goes down (Priority: P1)

**Acceptance Scenarios**:

1. **Given** an uptime monitor is configured, **When** `banana.engineer` returns non-200, **Then** an alert is sent within 5 minutes.

### User Story 2 — Engineer sees real-user CWV data after launch (Priority: P2)

**Acceptance Scenarios**:

1. **Given** Vercel Speed Insights is installed, **When** a user visits `banana.engineer`, **Then** LCP, CLS, and FID/INP data appears in the Vercel dashboard within 24 hours.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The observability stack MUST have zero or minimal cost impact on the project.
- **FR-002**: Error tracking MUST include source-map upload so minified stack traces are readable.
- **FR-003**: Uptime monitoring MUST cover at least the root URL and the API health endpoint.

## Success Criteria *(mandatory)*

### Validation Lane

```bash
python scripts/validate-spec-tasks-parity.py .specify/specs/120-observability-analytics-spike
```

## Assumptions

- Vercel Analytics and Speed Insights are available on the free Vercel Hobby plan.
- Sentry free tier (5k errors/month) is sufficient for early production traffic.
- User consent / cookie banner is out of scope for this spike but must be noted as a follow-up.
