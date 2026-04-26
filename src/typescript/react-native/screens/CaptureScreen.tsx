// React Native CaptureScreen (slice 025).
// Single-input text-paste entry. Renders the canonical empty-state
// copy and submits to the slice 017 endpoint via lib/api.

import {useState} from 'react';
import {ActivityIndicator, Pressable, SafeAreaView, ScrollView, Text, TextInput, View} from 'react-native';
import {fetchEnsembleVerdictWithEmbedding, type EnsembleVerdictWithEmbedding} from '../lib/api';
import {VERDICT_EMPTY_COPY, errorWording} from '../lib/copy';

export type CaptureScreenProps = {
  apiBaseUrl: string;
  initialDraft?: string;
  onVerdict: (payload: EnsembleVerdictWithEmbedding, sourceText: string) => void;
};

export function CaptureScreen({apiBaseUrl, initialDraft = '', onVerdict}: CaptureScreenProps) {
  const [draft, setDraft] = useState(initialDraft);
  const [submitting, setSubmitting] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const disabled = submitting || apiBaseUrl.length === 0 || draft.trim().length === 0;

  async function submit() {
    if (disabled) return;
    setSubmitting(true);
    setError(null);
    try {
      const payload = await fetchEnsembleVerdictWithEmbedding(apiBaseUrl, draft.trim());
      onVerdict(payload, draft.trim());
    } catch (err) {
      setError(errorWording(err));
    } finally {
      setSubmitting(false);
    }
  }

  return (
    <SafeAreaView style={{flex: 1, padding: 24}}>
      <ScrollView contentContainerStyle={{flexGrow: 1}}>
        <Text style={{fontSize: 22, fontWeight: '600', marginBottom: 12}} accessibilityRole="header">
          Banana classifier
        </Text>
        <Text testID="capture-empty-copy" style={{fontSize: 14, color: '#475569', marginBottom: 16}}>
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
            borderColor: '#cbd5e1',
            borderRadius: 8,
            padding: 10,
            fontSize: 14,
            backgroundColor: '#ffffff',
            textAlignVertical: 'top',
          }}
        />
        <Pressable
          testID="capture-submit"
          onPress={() => { void submit(); }}
          disabled={disabled}
          style={{
            marginTop: 12,
            borderRadius: 8,
            paddingVertical: 12,
            alignItems: 'center',
            backgroundColor: disabled ? '#cbd5e1' : '#65a30d',
          }}>
          <Text style={{color: '#ffffff', fontWeight: '600'}}>
            {submitting ? 'Classifying...' : 'Classify'}
          </Text>
        </Pressable>
        {submitting ? (
          <View style={{flexDirection: 'row', alignItems: 'center', marginTop: 8}}>
            <ActivityIndicator size="small" color="#65a30d" />
            <Text style={{marginLeft: 6, fontSize: 12, color: '#475569'}}>Working on it...</Text>
          </View>
        ) : null}
        {error ? (
          <Text testID="capture-error" style={{marginTop: 8, fontSize: 13, color: '#b91c1c'}}>
            {error}
          </Text>
        ) : null}
        {apiBaseUrl.length === 0 ? (
          <Text style={{marginTop: 8, fontSize: 12, color: '#b45309'}}>
            Set EXPO_PUBLIC_BANANA_API_BASE_URL to enable classification.
          </Text>
        ) : null}
      </ScrollView>
    </SafeAreaView>
  );
}
