# 021 -- Deferred Registry

## D-021-01 -- Dark mode token set

- **Disposition**: `defer-with-trigger`.
- **Trigger condition**: Slice 028 ships AND >=1 customer-facing
  request asks for dark mode OR an OS-level dark-mode signal needs
  to be honored on Electron / React Native.
- **Owner when triggered**: react-ui-agent + mobile-runtime-agent.
- **Rationale**: Dark mode requires a parallel token set and a
  runtime swap mechanism. The CSS-variable strategy in slice 026
  preserves the option to swap at root without component changes.

## D-021-02 -- Storybook adoption

- **Disposition**: `defer-with-trigger`.
- **Trigger condition**: Component count in `shared/ui` exceeds 12,
  OR a non-engineer (designer / PM) needs to review components in
  isolation.
- **Rationale**: 8 components is below the friction threshold where
  Storybook pays for itself. vitest snapshot coverage is sufficient
  for v1.

## D-021-03 -- Figma <-> token sync

- **Disposition**: `defer-with-trigger`.
- **Trigger condition**: A design tool source-of-truth exists
  upstream of the token TS module.
- **Rationale**: There is no Figma project today. Adding a sync
  pipeline now would be premature.

## D-021-04 -- Density variants (compact / comfortable)

- **Disposition**: `defer-with-trigger`.
- **Trigger condition**: Mobile customer feedback identifies finger-
  target issues OR desktop layouts overflow at default density.

## D-021-05 -- Component RTL / i18n token surface

- **Disposition**: `defer-with-trigger`.
- **Trigger condition**: First non-English locale ships.
