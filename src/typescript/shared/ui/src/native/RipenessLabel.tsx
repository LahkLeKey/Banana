// @banana/ui native annotation: React Native primitives only.
// Slice 027 -- token-driven.
import { Text } from "react-native";
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
    <Text
      style={{
        borderRadius: tokens.radius.sm,
        overflow: "hidden",
        backgroundColor: tone.bg,
        color: tone.fg,
        fontSize: tokens.font.size.xs,
        paddingHorizontal: tokens.space[2],
        paddingVertical: 2,
      }}
    >
      {value}
    </Text>
  );
}
