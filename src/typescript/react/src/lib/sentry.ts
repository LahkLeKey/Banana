// Sentry error tracking bootstrap.
// DSN is read from VITE_SENTRY_DSN env var.
// If absent, Sentry is disabled (dev/test mode).
export function initSentry(): void {
  const dsn = import.meta.env.VITE_SENTRY_DSN;
  if (!dsn) return;
  // Dynamic import so Sentry is tree-shaken when DSN is absent
  import('@sentry/react').then(({init, browserTracingIntegration}) => {
    init({
      dsn,
      integrations: [browserTracingIntegration()],
      tracesSampleRate: 0.1,
      environment: import.meta.env.MODE,
    });
  });
}
