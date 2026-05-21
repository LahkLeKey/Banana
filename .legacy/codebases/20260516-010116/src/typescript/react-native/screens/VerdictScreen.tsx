// React Native VerdictScreen (slice 025).
// Renders canonical verdict copy + escalation cue + lazy-loaded
// fingerprint via the shared native EscalationPanel.

import { type EmbeddingSummary, EscalationPanel, tokens } from "@banana/ui/native";
import { Pressable, SafeAreaView, ScrollView, Text, View } from "react-native";
import type { EnsembleVerdictWithEmbedding } from "../lib/api";
import { RETRY_BUTTON_COPY, verdictCopy } from "../lib/copy";

export type VerdictScreenProps = {
  payload: EnsembleVerdictWithEmbedding;
  onRetry: () => void;
};

export function VerdictScreen({ payload, onRetry }: VerdictScreenProps) {
  const { verdict, embedding } = payload;
  const copy = verdictCopy(verdict);

  async function loadSummary(): Promise<EmbeddingSummary> {
    return { embedding };
  }

  return (
    <SafeAreaView style={{ flex: 1, padding: tokens.space[6] }}>
      <ScrollView contentContainerStyle={{ flexGrow: 1 }}>
        <Text
          style={{
            fontSize: tokens.font.size.lg,
            fontWeight: tokens.font.weight.semibold,
            marginBottom: tokens.space[4],
          }}
          accessibilityRole="header"
        >
          Verdict
        </Text>
        <View
          testID="verdict-surface"
          style={{
            padding: tokens.space[4],
            borderRadius: tokens.radius.lg,
            backgroundColor:
              verdict.label === "banana" ? tokens.color.banana.bg : tokens.color.notbanana.bg,
          }}
        >
          <Text
            testID="verdict-copy"
            style={{
              fontSize: tokens.font.size.md,
              fontWeight: tokens.font.weight.semibold,
              color: tokens.color.text.default,
            }}
          >
            {copy}
          </Text>
          {verdict.did_escalate ? (
            <View testID="verdict-escalation-cue" style={{ marginTop: tokens.space[3] }}>
              <EscalationPanel loadSummary={loadSummary} />
            </View>
          ) : null}
        </View>
        <Pressable
          testID="verdict-retry"
          onPress={onRetry}
          style={{
            marginTop: tokens.space[4],
            paddingVertical: tokens.space[3],
            borderRadius: tokens.radius.md,
            alignItems: "center",
            // Slice 028 exception: sky CTA color kept; no CTA-color token shipped yet.
            backgroundColor: "#0ea5e9",
          }}
        >
          <Text
            style={{ color: tokens.color.surface.default, fontWeight: tokens.font.weight.semibold }}
          >
            {RETRY_BUTTON_COPY}
          </Text>
        </Pressable>
      </ScrollView>
    </SafeAreaView>
  );
}
