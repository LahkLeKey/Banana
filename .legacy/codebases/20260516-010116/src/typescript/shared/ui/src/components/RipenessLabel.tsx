// @banana/ui annotation: cross-platform.
// Slice 027 -- token-driven. Public props preserved.
import { tokens } from "../tokens";
import type { Ripeness } from "../types";

const TONE: Record<Ripeness, { bg: string; fg: string }> = {
  ripe: { bg: tokens.color.banana.bg, fg: tokens.color.banana.fg },
  unripe: { bg: tokens.color.surface.muted, fg: tokens.color.text.default },
  overripe: { bg: tokens.color.escalation.accent, fg: tokens.color.surface.default },
};

export function RipenessLabel({ value }: { value: Ripeness }) {
  const tone = TONE[value];
  return (
    <span
      style={{
        display: "inline-block",
        borderRadius: tokens.radius.sm,
        backgroundColor: tone.bg,
        color: tone.fg,
        paddingInline: tokens.space[2],
        paddingBlock: 2,
        fontSize: tokens.font.size.xs,
      }}
    >
      {value}
    </span>
  );
}
