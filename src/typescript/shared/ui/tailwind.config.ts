/** @type {import('tailwindcss').Config} */
import type {Config} from 'tailwindcss';

// Slice 026 -- map semantic Tailwind keys onto the CSS variables emitted
// by `src/tokens/web.css`. Existing class consumers keep working; new
// consumers should reach for these semantic names instead of raw
// palette references.
const config: Config = {
  content: ['./src/**/*.{ts,tsx}'],
  theme: {
    extend: {
      colors: {
        surface: {
          DEFAULT: 'var(--color-surface-default)',
          muted: 'var(--color-surface-muted)',
        },
        text: {
          DEFAULT: 'var(--color-text-default)',
          muted: 'var(--color-text-muted)',
          error: 'var(--color-text-error)',
        },
        banana: {
          bg: 'var(--color-banana-bg)',
          fg: 'var(--color-banana-fg)',
        },
        notbanana: {
          bg: 'var(--color-notbanana-bg)',
          fg: 'var(--color-notbanana-fg)',
        },
        escalation: {
          bg: 'var(--color-escalation-bg)',
          fg: 'var(--color-escalation-fg)',
          accent: 'var(--color-escalation-accent)',
        },
      },
      borderRadius: {
        sm: 'var(--radius-sm)',
        md: 'var(--radius-md)',
        lg: 'var(--radius-lg)',
        pill: 'var(--radius-pill)',
      },
      transitionDuration: {
        fast: 'var(--motion-fast)',
        medium: 'var(--motion-medium)',
      },
    },
  },
  plugins: [],
};
export default config;
