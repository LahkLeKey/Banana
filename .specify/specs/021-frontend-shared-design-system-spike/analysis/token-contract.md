# 021 -- Token Contract Proposal

Minimum-viable token set required to unblock SPIKE 020 follow-up
slices (023, 024, 025) and the SPIKE 022 offline UX surfaces.

## Color tokens (semantic, not raw)

| Token | Web reference | Native reference | Used by |
|-------|---------------|------------------|---------|
| `color.surface.default` | `#ffffff` | `#ffffff` | All cards, panels |
| `color.surface.muted` | `#f8fafc` | `#f8fafc` | Chat backdrop |
| `color.text.default` | `#0f172a` | `#0f172a` | Body |
| `color.text.muted` | `#64748b` | `#64748b` | Secondary text, error context |
| `color.text.error` | `#b91c1c` | `#b91c1c` | Error wording (baseline) |
| `color.banana.bg` | `#fef3c7` | `#fef3c7` | Banana verdict surface |
| `color.banana.fg` | `#78350f` | `#78350f` | Banana verdict text |
| `color.notbanana.bg` | `#e2e8f0` | `#e2e8f0` | Not-banana verdict surface |
| `color.notbanana.fg` | `#0f172a` | `#0f172a` | Not-banana verdict text |
| `color.escalation.bg` | `#fef3c7` | `#fef3c7` | Escalation cue background |
| `color.escalation.fg` | `#92400e` | `#92400e` | Escalation cue text (replaces baseline `amber-700`/`#b45309` literal) |
| `color.escalation.accent` | `#b45309` | `#b45309` | Escalation underline / icon |

## Typography tokens

| Token | Size | Weight | Used by |
|-------|------|--------|---------|
| `font.size.xs` | 12 | 400 | Captions, error wording |
| `font.size.sm` | 14 | 400 | Body |
| `font.size.md` | 16 | 500 | Verdict copy |
| `font.size.lg` | 22 | 600 | Headings |
| `font.weight.regular` | -- | 400 | Body |
| `font.weight.medium` | -- | 500 | Buttons, badges |
| `font.weight.semibold` | -- | 600 | Verdict copy, headings |

## Spacing tokens (4-pt grid)

`space.0=0`, `space.1=4`, `space.2=8`, `space.3=12`, `space.4=16`,
`space.5=20`, `space.6=24`. Limit to these values.

## Radius tokens

`radius.sm=4`, `radius.md=8`, `radius.lg=12`, `radius.pill=999`.

## Motion tokens

`motion.fast=120ms`, `motion.medium=240ms`. Used by escalation panel
open/close.

## Justification (mapped to SPIKE 020 / 022 needs)

- Verdict copy + escalation cue (slices 023-025) need
  `color.banana.*`, `color.notbanana.*`, `color.escalation.*`,
  `font.size.md`, `font.weight.semibold`.
- Inline retry button (slices 023-025) needs `color.text.error`,
  `font.size.xs`, `space.2`, `radius.md`.
- Escalation panel open/close transition (slices 023, 025) needs
  `motion.medium`.
- Offline / queued cue (SPIKE 022 follow-ups) will need
  `color.text.muted`, `space.1..3`. No new tokens required.

## Out of contract

- Dark mode (deferred -- requires a second token set).
- Density variants (compact / comfortable).
- Per-channel overrides beyond the web/native split below.
