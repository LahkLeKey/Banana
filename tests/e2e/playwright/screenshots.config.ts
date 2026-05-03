/**
 * Screenshot manifest — each entry defines one capture.
 * The `name` field maps directly to `docs/screenshots/<name>.png`.
 */
export interface ScreenshotCapture {
  /** Output filename (no extension). Written to docs/screenshots/<name>.png */
  name: string;
  /** Route path to navigate to before capture */
  route: string;
  /** Human-readable label used in the README gallery */
  label: string;
  /** Panel group used for gallery grouping in README */
  group: "ensemble" | "chat" | "ripeness" | "error-boundary" | "app";
  /** Whether to capture full-page or viewport */
  fullPage: boolean;
  /** Optional CSS selector to clip the screenshot to a specific region */
  clip?: string;
}

export const SCREENSHOTS: ScreenshotCapture[] = [
  // ── App shell ───────────────────────────────────────────────────────────────
  {
    name: "app-shell-idle",
    route: "/classify",
    label: "App Shell — Idle",
    group: "app",
    fullPage: false,
  },

  // ── Ensemble panel ──────────────────────────────────────────────────────────
  {
    name: "ensemble-form-idle",
    route: "/classify",
    label: "Ensemble — Form Idle",
    group: "ensemble",
    fullPage: false,
    clip: "[data-testid='ensemble-form']",
  },
  {
    name: "ensemble-verdict-success",
    route: "/classify",
    label: "Ensemble — Verdict Success",
    group: "ensemble",
    fullPage: false,
    clip: "[data-testid='ensemble-verdict-surface']",
  },
  {
    name: "ensemble-verdict-error",
    route: "/classify",
    label: "Ensemble — Verdict Error",
    group: "ensemble",
    fullPage: false,
    clip: "[data-testid='ensemble-verdict-surface']",
  },
  {
    name: "ensemble-recent-verdicts",
    route: "/classify",
    label: "Ensemble — Recent Verdicts",
    group: "ensemble",
    fullPage: false,
    clip: "[data-testid='recent-verdicts']",
  },

  // ── Chat panel ──────────────────────────────────────────────────────────────
  {
    name: "chat-empty",
    route: "/classify",
    label: "Chat — Empty",
    group: "chat",
    fullPage: false,
  },
  {
    name: "chat-with-messages",
    route: "/classify",
    label: "Chat — With Messages",
    group: "chat",
    fullPage: false,
  },

  // ── Ripeness panel ──────────────────────────────────────────────────────────
  {
    name: "ripeness-idle",
    route: "/classify",
    label: "Ripeness — Idle",
    group: "ripeness",
    fullPage: false,
  },
  {
    name: "ripeness-result",
    route: "/classify",
    label: "Ripeness — Result",
    group: "ripeness",
    fullPage: false,
  },

  // ── Error boundary ──────────────────────────────────────────────────────────
  {
    name: "error-boundary-fallback",
    route: "/classify",
    label: "Error Boundary — Fallback Card",
    group: "error-boundary",
    fullPage: false,
  },

  // ── Full-page scroll ────────────────────────────────────────────────────────
  {
    name: "full-page-classify",
    route: "/classify",
    label: "Full Page — /classify",
    group: "app",
    fullPage: true,
  },
];
