// Slice 026 -- single source of truth for design tokens.
// Contract pinned by
// .specify/specs/021-frontend-shared-design-system-spike/analysis/token-contract.md
// Web consumes via the generated tokens/web.css; React Native consumes
// the object directly through @banana/ui/native/tokens.

export const tokens = {
  color: {
    surface: {
      default: "#ffffff",
      muted: "#f8fafc",
    },
    text: {
      default: "#0f172a",
      muted: "#64748b",
      error: "#b91c1c",
    },
    banana: {
      bg: "#fef3c7",
      fg: "#78350f",
    },
    notbanana: {
      bg: "#e2e8f0",
      fg: "#0f172a",
    },
    escalation: {
      bg: "#fef3c7",
      fg: "#92400e",
      accent: "#b45309",
    },
  },
  font: {
    size: {
      xs: 12,
      sm: 14,
      md: 16,
      lg: 22,
    },
    weight: {
      regular: "400",
      medium: "500",
      semibold: "600",
    },
  },
  space: {
    0: 0,
    1: 4,
    2: 8,
    3: 12,
    4: 16,
    5: 20,
    6: 24,
  },
  radius: {
    sm: 4,
    md: 8,
    lg: 12,
    pill: 999,
  },
  motion: {
    fast: "120ms",
    medium: "240ms",
  },
} as const;

export type Tokens = typeof tokens;
