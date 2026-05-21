// @banana/ui native annotation: React Native primitives only.
// Slice 027 -- token-driven; adds ensemble variant (closes inventory gap).
import type { ReactNode } from "react";
import { Text, View } from "react-native";
import { tokens } from "../tokens";
import type { EnsembleVerdict } from "../types";

type CountVariantProps = {
  variant?: "count";
  count: number;
  children?: ReactNode;
};

type EnsembleVariantProps = {
  variant: "ensemble";
  verdict: EnsembleVerdict;
  showAttention?: boolean;
  children?: ReactNode;
};

export type BananaBadgeProps = CountVariantProps | EnsembleVariantProps;

export function BananaBadge(props: BananaBadgeProps) {
  if (props.variant === "ensemble") {
    return <EnsembleBadge {...props} />;
  }
  const { count, children } = props;
  return (
    <View
      testID="banana-badge"
      style={{
        flexDirection: "row",
        alignItems: "center",
        borderRadius: tokens.radius.md,
        backgroundColor: tokens.color.banana.bg,
        paddingHorizontal: tokens.space[2],
        paddingVertical: tokens.space[1],
        gap: tokens.space[1],
      }}
    >
      <Text
        style={{
          fontSize: tokens.font.size.xs,
          fontWeight: tokens.font.weight.semibold,
          color: tokens.color.banana.fg,
        }}
      >
        🍌 {count}
      </Text>
      {children ? (
        <Text style={{ fontSize: tokens.font.size.xs, color: tokens.color.banana.fg }}>
          {children}
        </Text>
      ) : null}
    </View>
  );
}

function EnsembleBadge({ verdict, showAttention, children }: EnsembleVariantProps) {
  const labelText =
    verdict.label === "banana"
      ? "banana"
      : verdict.label === "not_banana"
        ? "not banana"
        : "unknown";
  const tone =
    verdict.label === "banana"
      ? { bg: tokens.color.banana.bg, fg: tokens.color.banana.fg }
      : verdict.label === "not_banana"
        ? { bg: tokens.color.notbanana.bg, fg: tokens.color.notbanana.fg }
        : { bg: tokens.color.escalation.bg, fg: tokens.color.text.error };
  const rawMagnitude =
    typeof verdict.calibration_magnitude === "number" ? verdict.calibration_magnitude : Number.NaN;
  const magnitude = Number.isFinite(rawMagnitude) ? rawMagnitude.toFixed(2) : "--";

  return (
    <View
      testID="banana-badge-ensemble"
      style={{
        flexDirection: "row",
        alignItems: "center",
        borderRadius: tokens.radius.md,
        backgroundColor: tone.bg,
        paddingHorizontal: tokens.space[2],
        paddingVertical: tokens.space[1],
        gap: tokens.space[2],
      }}
    >
      <Text
        style={{
          fontSize: tokens.font.size.xs,
          fontWeight: tokens.font.weight.medium,
          color: tone.fg,
        }}
      >
        🍌 {labelText}
      </Text>
      <Text
        testID="banana-badge-ensemble-magnitude"
        style={{ fontSize: tokens.font.size.xs, color: tone.fg, opacity: 0.75 }}
      >
        m={magnitude}
      </Text>
      {verdict.did_escalate ? (
        <Text
          testID="banana-badge-ensemble-escalated"
          style={{
            backgroundColor: tokens.color.escalation.bg,
            color: tokens.color.escalation.fg,
            borderRadius: tokens.radius.pill,
            paddingHorizontal: tokens.space[1],
            fontSize: 10,
            fontWeight: tokens.font.weight.semibold,
            textTransform: "uppercase",
            overflow: "hidden",
          }}
        >
          escalated
        </Text>
      ) : null}
      {showAttention ? (
        <Text
          testID="banana-badge-ensemble-attention"
          style={{
            backgroundColor: tokens.color.surface.muted,
            color: tokens.color.text.muted,
            borderRadius: tokens.radius.pill,
            paddingHorizontal: tokens.space[1],
            fontSize: 10,
            fontWeight: tokens.font.weight.semibold,
            textTransform: "uppercase",
            overflow: "hidden",
          }}
        >
          {verdict.did_escalate ? "fb" : "rb"}
        </Text>
      ) : null}
      {children}
    </View>
  );
}
