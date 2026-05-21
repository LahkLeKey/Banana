// React Native CaptureScreen (slice 025).
// Single-input text-paste entry. Renders the canonical empty-state
// copy and submits to the slice 017 endpoint via lib/api.

import { tokens } from "@banana/ui/native";
import { useState } from "react";
import {
  ActivityIndicator,
  Pressable,
  SafeAreaView,
  ScrollView,
  Text,
  TextInput,
  View,
} from "react-native";
import { type EnsembleVerdictWithEmbedding, fetchEnsembleVerdictWithEmbedding } from "../lib/api";
import { errorWording, VERDICT_EMPTY_COPY } from "../lib/copy";
import {
  ENSEMBLE_RETRY_POLICY,
  type EnsembleQueuePayload,
  getEnsembleQueue,
  getVerdictHistory,
} from "../lib/resilience-bootstrap";

export type CaptureScreenProps = {
  apiBaseUrl: string;
  initialDraft?: string;
  onVerdict: (payload: EnsembleVerdictWithEmbedding, sourceText: string) => void;
  onShowHistory?: () => void;
};

export function CaptureScreen({
  apiBaseUrl,
  initialDraft = "",
  onVerdict,
  onShowHistory,
}: CaptureScreenProps) {
  const [draft, setDraft] = useState(initialDraft);
  const [submitting, setSubmitting] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [queuedNotice, setQueuedNotice] = useState(false);

  const disabled = submitting || apiBaseUrl.length === 0 || draft.trim().length === 0;

  async function submit() {
    if (disabled) return;
    const sample = draft.trim();
    setSubmitting(true);
    setError(null);
    setQueuedNotice(false);
    try {
      const payload = await fetchEnsembleVerdictWithEmbedding(apiBaseUrl, sample);
      onVerdict(payload, sample);
      // Slice 031 -- record verdict to history (best-effort).
      try {
        await getVerdictHistory().record({
          id: `v_${Date.now()}_${Math.random().toString(36).slice(2, 8)}`,
          capturedAt: Date.now(),
          input: sample,
          verdict: payload.verdict,
          didEscalate: Boolean(payload.verdict.did_escalate),
        });
      } catch {
        /* history is best-effort */
      }
    } catch (err) {
      setError(errorWording(err));
      // Slice 031 -- queue the sample so a NetInfo-driven drain
      // can resolve it after reconnection. The draft is
      // preserved by virtue of `draft` state staying intact.
      try {
        await getEnsembleQueue().enqueue({
          key: `ensemble:${sample}`,
          payload: { sample, submittedAt: Date.now() } satisfies EnsembleQueuePayload,
          retry: ENSEMBLE_RETRY_POLICY,
          enqueuedAt: Date.now(),
        });
        setQueuedNotice(true);
      } catch {
        /* queue is best-effort */
      }
    } finally {
      setSubmitting(false);
    }
  }

  return (
    <SafeAreaView style={{ flex: 1, padding: tokens.space[6] }}>
      <ScrollView contentContainerStyle={{ flexGrow: 1 }}>
        <Text
          style={{
            fontSize: tokens.font.size.lg,
            fontWeight: tokens.font.weight.semibold,
            marginBottom: tokens.space[3],
          }}
          accessibilityRole="header"
        >
          Banana classifier
        </Text>
        <Text
          testID="capture-empty-copy"
          style={{
            fontSize: tokens.font.size.sm,
            color: tokens.color.text.muted,
            marginBottom: tokens.space[4],
          }}
        >
          {VERDICT_EMPTY_COPY}
        </Text>
        <TextInput
          testID="capture-input"
          value={draft}
          onChangeText={setDraft}
          placeholder="Paste a sample"
          multiline
          editable={!submitting}
          style={{
            minHeight: 96,
            borderWidth: 1,
            // Slice 028 exception: neutral input border kept as a slate hex; no border token shipped yet.
            borderColor: "#cbd5e1",
            borderRadius: tokens.radius.md,
            padding: tokens.space[3],
            fontSize: tokens.font.size.sm,
            backgroundColor: tokens.color.surface.default,
            textAlignVertical: "top",
          }}
        />
        <Pressable
          testID="capture-submit"
          onPress={() => {
            void submit();
          }}
          disabled={disabled}
          style={{
            marginTop: tokens.space[3],
            borderRadius: tokens.radius.md,
            paddingVertical: tokens.space[3],
            alignItems: "center",
            // Slice 028 exception: lime CTA / disabled slate kept; no CTA-color token shipped yet.
            backgroundColor: disabled ? "#cbd5e1" : "#65a30d",
          }}
        >
          <Text
            style={{ color: tokens.color.surface.default, fontWeight: tokens.font.weight.semibold }}
          >
            {submitting ? "Classifying..." : "Classify"}
          </Text>
        </Pressable>
        {submitting ? (
          <View style={{ flexDirection: "row", alignItems: "center", marginTop: tokens.space[2] }}>
            <ActivityIndicator size="small" color="#65a30d" />
            <Text
              style={{
                marginLeft: tokens.space[1],
                fontSize: tokens.font.size.xs,
                color: tokens.color.text.muted,
              }}
            >
              Working on it...
            </Text>
          </View>
        ) : null}
        {error ? (
          <Text
            testID="capture-error"
            style={{ marginTop: tokens.space[2], fontSize: 13, color: tokens.color.text.error }}
          >
            {error}
          </Text>
        ) : null}
        {queuedNotice ? (
          <Text
            testID="capture-queued-notice"
            style={{
              marginTop: tokens.space[1],
              fontSize: tokens.font.size.xs,
              color: tokens.color.text.muted,
            }}
          >
            Saved your sample. We’ll classify it when you’re back online.
          </Text>
        ) : null}
        {apiBaseUrl.length === 0 ? (
          <Text
            style={{
              marginTop: tokens.space[2],
              fontSize: tokens.font.size.xs,
              color: tokens.color.escalation.accent,
            }}
          >
            Set EXPO_PUBLIC_BANANA_API_BASE_URL to enable classification.
          </Text>
        ) : null}
        {onShowHistory ? (
          <Pressable
            testID="capture-history-link"
            onPress={onShowHistory}
            style={{ marginTop: tokens.space[4], alignItems: "center" }}
          >
            <Text
              style={{
                color: tokens.color.text.muted,
                fontSize: tokens.font.size.sm,
                textDecorationLine: "underline",
              }}
            >
              Recent verdicts
            </Text>
          </Pressable>
        ) : null}
      </ScrollView>
    </SafeAreaView>
  );
}
