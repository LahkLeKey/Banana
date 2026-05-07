# 021 -- Component Inventory

Audit of `src/typescript/shared/ui` exports and per-channel consumption
as of 2026-04-26.

## Exports (from `src/typescript/shared/ui/src/index.ts`)

- `BananaBadge` (web variant: `components/BananaBadge.tsx`; native variant: `native/BananaBadge.tsx` via `@banana/ui/native`)
- `ChatMessageBubble`
- `RipenessLabel`
- Types: `ChatMessage`, `ChatMessageStatus`, `ChatRole`, `ChatSession`, `EnsembleLabel`, `EnsembleStatus`, `EnsembleVerdict`, `Ripeness`

## Component x channel x styling-source matrix

| Component | Web variant styling | Native variant styling | React consumes | Electron consumes | RN consumes |
|-----------|---------------------|------------------------|----------------|-------------------|-------------|
| `BananaBadge` (count) | Tailwind classes (`bg-yellow-200`, `text-yellow-900`) hard-coded | Inline `View` style with hex literals (`#fde68a`, `#78350f`) | YES | (proxies React) | YES (stub `count={0}`) |
| `BananaBadge` (ensemble) | Tailwind classes; conditional tones via ternary | NOT IMPLEMENTED for native | YES | (proxies React) | NO |
| `ChatMessageBubble` | Tailwind | Inline `View`/`Text` styles | YES | (proxies React) | YES |
| `RipenessLabel` | Tailwind `COLORS` record (3 hex-equivalent token names) | NOT IMPLEMENTED for native | YES | (proxies React) | YES (stub literal) |

## Findings

1. **Two parallel styling systems.** Web variants use Tailwind utility classes; native variants use raw hex literals in inline `View`/`Text` styles. There is no shared source of truth for color (`bg-yellow-200` on web vs `#fde68a` on RN -- which happen to be the same color today, but only by coincidence).
2. **Missing native variants.** `BananaBadge` ensemble variant and `RipenessLabel` have no native implementations at all. RN today renders only the count variant of `BananaBadge` and a hard-coded literal for `RipenessLabel`.
3. **Hard-coded color literals.** Both web (`bg-yellow-200`, `bg-amber-700`, `bg-rose-100`) and native (`#fde68a`, `#78350f`, `#b45309`) embed colors directly in component source. There is no token layer.
4. **No spacing / typography scale.** Spacing (`px-2 py-1`, `paddingHorizontal: 8`) and font sizes (`text-xs`, `fontSize: 12`) are written inline.
5. **No motion / radius scale.** `rounded-md`, `borderRadius: 8` are inline.
6. **SPIKE 020 follow-ups will need new components.** `EscalationPanel` (web + native variants) is named in slices 023 and 025; it cannot land coherently across both channels without a token contract.

## Implication

The `shared/ui` package is operating as "two unrelated implementations
of the same name" rather than as a design system. Any new shared
component will compound the drift unless tokens land first.
