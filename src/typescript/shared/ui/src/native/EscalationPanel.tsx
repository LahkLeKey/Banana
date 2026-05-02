// @banana/ui native annotation: React Native primitives only.
// Slice 025 -- React Native variant of slice 023's web EscalationPanel.
// Same lazy-load + 4-component fingerprint contract; primitives differ.

import { useState } from "react";
import { ActivityIndicator, Pressable, Text, View } from "react-native";
import type { EmbeddingSummary, EscalationPanelProps } from "../components/EscalationPanel";
import { tokens } from "../tokens";

export type { EmbeddingSummary, EscalationPanelProps } from "../components/EscalationPanel";

const COMPONENT_LABELS = [
  "banana_context",
  "not_banana_context",
  "attention_delta",
  "banana_probability",
];

export function EscalationPanel({ loadSummary, triggerLabel = "Why?" }: EscalationPanelProps) {
  const [open, setOpen] = useState(false);
  const [summary, setSummary] = useState<EmbeddingSummary | null>(null);
  const [error, setError] = useState<string | null>(null);
  const [loading, setLoading] = useState(false);

  async function toggle() {
    if (open) {
      setOpen(false);
      return;
    }
    setOpen(true);
    if (summary || loading) return;
    setLoading(true);
    setError(null);
    try {
      setSummary(await loadSummary());
    } catch (err: unknown) {
      setError(err instanceof Error ? err.message : "Couldn't load the details. Try again.");
    } finally {
      setLoading(false);
    }
  }

  return (
    <View testID="escalation-panel" style={{ marginTop: tokens.space[2] }}>
      <Pressable
        testID="escalation-panel-trigger"
        onPress={() => {
          void toggle();
        }}
        accessibilityState={{ expanded: open }}
      >
        <Text
          style={{
            color: tokens.color.escalation.accent,
            textDecorationLine: "underline",
            fontSize: tokens.font.size.xs,
            fontWeight: tokens.font.weight.semibold,
          }}
        >
          {triggerLabel}
        </Text>
      </Pressable>
      {open ? (
        <View
          testID="escalation-panel-body"
          style={{
            marginTop: tokens.space[2],
            padding: tokens.space[3],
            borderRadius: tokens.radius.md,
            borderWidth: 1,
            borderColor: tokens.color.escalation.accent,
            backgroundColor: tokens.color.escalation.bg,
          }}
        >
          {loading ? (
            <View
              testID="escalation-panel-loading"
              style={{ flexDirection: "row", alignItems: "center" }}
            >
              <ActivityIndicator size="small" color={tokens.color.escalation.accent} />
              <Text
                style={{
                  marginLeft: tokens.space[1],
                  fontSize: tokens.font.size.xs,
                  color: tokens.color.escalation.fg,
                }}
              >
                Loading...
              </Text>
            </View>
          ) : error ? (
            <Text
              testID="escalation-panel-error"
              style={{ fontSize: tokens.font.size.xs, color: tokens.color.text.error }}
            >
              {error}
            </Text>
          ) : summary ? (
            <DefaultSummary summary={summary} />
          ) : null}
        </View>
      ) : null}
    </View>
  );
}

function DefaultSummary({ summary }: { summary: EmbeddingSummary }) {
  if (!summary.embedding) {
    return (
      <Text
        testID="escalation-panel-empty"
        style={{ fontSize: tokens.font.size.xs, color: tokens.color.escalation.fg }}
      >
        No fingerprint captured for this verdict.
      </Text>
    );
  }
  return (
    <View testID="escalation-panel-fingerprint">
      {summary.embedding.map((value, index) => (
        <View
          key={index}
          style={{ flexDirection: "row", justifyContent: "space-between", paddingVertical: 2 }}
        >
          <Text
            style={{
              fontSize: tokens.font.size.xs,
              color: tokens.color.escalation.fg,
              fontWeight: tokens.font.weight.medium,
            }}
          >
            {COMPONENT_LABELS[index] ?? `dim_${index}`}
          </Text>
          <Text style={{ fontSize: tokens.font.size.xs, color: tokens.color.escalation.fg }}>
            {value.toFixed(4)}
          </Text>
        </View>
      ))}
    </View>
  );
}
