## Problem Statement

## In Scope

- Feature implementation and integration
- Testing and validation of new behavior
- Documentation of feature usage and design decisions

Feature Specification: Frontend Shared Design System SPIKE aims to improve system capability and user experience by implementing the feature described in the specification.

# Feature Specification: Frontend Shared Design System SPIKE

**Feature Branch**: `[021-frontend-shared-design-system-spike]`
**Created**: 2026-04-26
**Status**: Draft -- ready to investigate
**Type**: SPIKE (investigate -> plan a small family of bounded follow-up specs)
**Triggered by**: Strategic refocus to customer-facing value. The
shared UI package (`src/typescript/shared/ui`) currently exposes three
ad-hoc components (`BananaBadge`, `ChatMessageBubble`, `RipenessLabel`)
with channel-specific styling assumptions. As we invest in coherent
React + Electron + React Native customer experiences (SPIKE 020),
these components will diverge unless we promote `shared/ui` to a real
design system with tokens, theming, and per-channel adoption rules.

## Problem

`src/typescript/shared/ui` was created as a convenience package for
the React badge work (slice 015). It now risks two failure modes:

1. **Drift between channels**: Electron and React Native do not yet
   consume `shared/ui`; if SPIKE 020's follow-up slices each style
   verdicts independently, the three channels will land with
   different colors, typography, spacing, and motion.
2. **Token-free expansion**: Any new shared component (escalation
   banner, error banner, loading affordance) added without a token
   contract will hard-code colors / spacing and force per-channel
   forks the moment any channel needs a theme override (dark mode,
   accessibility contrast, mobile density).

We need a deliberate decision now -- before SPIKE 020's follow-up
slices ship -- on whether `shared/ui` graduates to a tokenized design
system, and what the minimum viable token + component contract is.

## Investigation goals

- **G1**: Component inventory. Audit current `shared/ui` exports,
  their styling primitives (Tailwind classes, inline styles, raw
  CSS), and which channels actually consume them today. Output:
  component x channel x styling-source matrix.
- **G2**: Token gap analysis. Identify the smallest token set that
  unblocks the SPIKE 020 follow-ups (color, typography scale,
  spacing scale, radius, motion). Output: token contract proposal.
- **G3**: Pick exactly three follow-up implementation slices that
  graduate `shared/ui`:
  - **A**: Tokens + theme primitives (the contract).
  - **B**: Component v2 (BananaBadge, RipenessLabel,
    ChatMessageBubble, plus the new escalation + error banners
    needed by SPIKE 020 slices).
  - **C**: Per-channel adoption (React, Electron, React Native each
    consume the v2 components via the token contract).
- **G4**: Produce a readiness packet for each follow-up slice
  (slice 026, 027, 028) that the implementation slices can pick up
  without re-litigation.

## Out of Scope

- Implementing tokens, components, or per-channel adoption (slices
  026-028).
- Customer journey or verdict copy decisions (SPIKE 020 owns those).
- Offline / persistence design (SPIKE 022).
- New design tooling (Figma, Storybook) unless the SPIKE concludes
  it is the only viable path -- in which case it is called out as a
  deferred follow-up with an explicit trigger.

## Deliverables

- `analysis/component-inventory.md` -- component x channel x
  styling-source matrix.
- `analysis/token-contract.md` -- proposed minimum-viable token set
  with rationale tied to SPIKE 020 follow-up needs.
- `analysis/theming-strategy.md` -- how tokens flow into each channel
  (CSS variables for React/Electron, JS object for React Native, or
  shared TS export consumed by both).
- `analysis/followup-readiness-tokens.md` -- bounded scope for slice
  026 (token + theme primitive contract).
- `analysis/followup-readiness-components-v2.md` -- bounded scope for
  slice 027 (component v2 + new escalation/error banners).
- `analysis/followup-readiness-channel-adoption.md` -- bounded scope
  for slice 028 (per-channel adoption).
- `analysis/deferred-registry.md` -- any items pushed past slice 028.

## Success Criteria

- Inventory covers every export under `src/typescript/shared/ui/src/`.
- Token contract is small enough to be implemented in <=15 tasks.
- Theming strategy is explicit on how React Native consumes tokens
  (its styling primitive is not CSS).
- The three readiness packets are sequenced so that slice 027 cannot
  start until slice 026 lands, and slice 028 cannot start until 027
  lands.
- No code or contract changes land in this SPIKE.

## Validation lane

```
# SPIKE-only -- no build/test required.
# Verification is human review of the seven deliverables above.
ls .specify/specs/021-frontend-shared-design-system-spike/analysis/
```

## Downstream

This SPIKE unlocks three sequenced implementation slices:

- **026 Shared UI Tokens + Theme Primitives** -- the token contract.
- **027 Shared UI Components v2** -- component refresh + new
  escalation / error banners that consume tokens.
- **028 Per-Channel Design System Adoption** -- React, Electron, and
  React Native each consume the v2 components via the token contract.

## In-scope files

- `src/typescript/shared/ui/**` (READ ONLY -- audit input).
- `src/typescript/react/**`, `src/typescript/electron/**`,
  `src/typescript/react-native/**` (READ ONLY -- audit input for
  current consumption / styling overrides).
- `.specify/specs/021-frontend-shared-design-system-spike/analysis/**`
  (NEW -- SPIKE outputs).
- `.specify/feature.json` (repoint to 021 during execution).
