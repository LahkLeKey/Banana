// @banana/ui native annotation: React Native primitives only.
// Slice 025 -- React Native variant of slice 023's web EscalationPanel.
// Same lazy-load + 4-component fingerprint contract; primitives differ.

import {useState} from 'react';
import {ActivityIndicator, Pressable, Text, View} from 'react-native';
import type {EmbeddingSummary, EscalationPanelProps} from '../components/EscalationPanel';

export type {EmbeddingSummary, EscalationPanelProps} from '../components/EscalationPanel';

const COMPONENT_LABELS = [
  'banana_context',
  'not_banana_context',
  'attention_delta',
  'banana_probability',
];

export function EscalationPanel({
  loadSummary,
  triggerLabel = 'Why?',
}: EscalationPanelProps) {
  const [open, setOpen] = useState(false);
  const [summary, setSummary] = useState<EmbeddingSummary | null>(null);
  const [error, setError] = useState<string | null>(null);
  const [loading, setLoading] = useState(false);

  async function toggle() {
    if (open) { setOpen(false); return; }
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
    <View testID="escalation-panel" style={{marginTop: 8}}>
      <Pressable
        testID="escalation-panel-trigger"
        onPress={() => { void toggle(); }}
        accessibilityState={{expanded: open}}>
        <Text style={{color: '#b45309', textDecorationLine: 'underline', fontSize: 12, fontWeight: '600'}}>
          {triggerLabel}
        </Text>
      </Pressable>
      {open ? (
        <View
          testID="escalation-panel-body"
          style={{marginTop: 8, padding: 10, borderRadius: 6, borderWidth: 1, borderColor: '#fde68a', backgroundColor: '#fffbeb'}}>
          {loading ? (
            <View testID="escalation-panel-loading" style={{flexDirection: 'row', alignItems: 'center'}}>
              <ActivityIndicator size="small" color="#b45309" />
              <Text style={{marginLeft: 6, fontSize: 12, color: '#92400e'}}>Loading...</Text>
            </View>
          ) : error ? (
            <Text testID="escalation-panel-error" style={{fontSize: 12, color: '#b91c1c'}}>{error}</Text>
          ) : summary ? (
            <DefaultSummary summary={summary} />
          ) : null}
        </View>
      ) : null}
    </View>
  );
}

function DefaultSummary({summary}: {summary: EmbeddingSummary}) {
  if (!summary.embedding) {
    return (
      <Text testID="escalation-panel-empty" style={{fontSize: 12, color: '#92400e'}}>
        No fingerprint captured for this verdict.
      </Text>
    );
  }
  return (
    <View testID="escalation-panel-fingerprint">
      {summary.embedding.map((value, index) => (
        <View key={index} style={{flexDirection: 'row', justifyContent: 'space-between', paddingVertical: 2}}>
          <Text style={{fontSize: 12, color: '#92400e', fontWeight: '500'}}>
            {COMPONENT_LABELS[index] ?? `dim_${index}`}
          </Text>
          <Text style={{fontSize: 12, color: '#92400e'}}>
            {value.toFixed(4)}
          </Text>
        </View>
      ))}
    </View>
  );
}
