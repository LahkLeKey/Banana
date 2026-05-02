// React Native entry (specs 011 + 025 + 031).
// Slice 031 -- adds NetInfo-driven queue drain + HistoryScreen.

import { registerRootComponent } from "expo";
import { StatusBar } from "expo-status-bar";
import { useEffect, useState } from "react";
import { Linking } from "react-native";
import { type EnsembleVerdictWithEmbedding, fetchEnsembleVerdictWithEmbedding } from "./lib/api";
import { getEnsembleQueue, getVerdictHistory, onOnline } from "./lib/resilience-bootstrap";
import { CaptureScreen } from "./screens/CaptureScreen";
import { HistoryScreen } from "./screens/HistoryScreen";
import { VerdictScreen } from "./screens/VerdictScreen";
import { registerShareListener } from "./share-handler";

const API_BASE_URL = process.env.EXPO_PUBLIC_BANANA_API_BASE_URL ?? "";

type Route = "capture" | "verdict" | "history";

function App() {
  const [route, setRoute] = useState<Route>("capture");
  const [verdict, setVerdict] = useState<EnsembleVerdictWithEmbedding | null>(null);
  const [draft, setDraft] = useState("");

  useEffect(() => {
    return registerShareListener(Linking, (event) => {
      setDraft(event.text);
      setVerdict(null);
      setRoute("capture");
    });
  }, []);

  // Slice 031 -- NetInfo-driven drain of the offline queue.
  useEffect(() => {
    if (!API_BASE_URL) return;
    return onOnline(() => {
      void getEnsembleQueue()
        .drain(async (payload) => {
          const result = await fetchEnsembleVerdictWithEmbedding(API_BASE_URL, payload.sample);
          setVerdict(result);
          setRoute("verdict");
          try {
            await getVerdictHistory().record({
              id: `v_${Date.now()}_${Math.random().toString(36).slice(2, 8)}`,
              capturedAt: Date.now(),
              input: payload.sample,
              verdict: result.verdict,
              didEscalate: Boolean(result.verdict.did_escalate),
            });
          } catch {
            /* history is best-effort */
          }
        })
        .catch(() => {
          /* drain errors leave the job queued for next online tick */
        });
    });
  }, []);

  if (route === "history") {
    return (
      <>
        <StatusBar style="auto" />
        <HistoryScreen onBack={() => setRoute(verdict ? "verdict" : "capture")} />
      </>
    );
  }

  if (route === "verdict" && verdict) {
    return (
      <>
        <StatusBar style="auto" />
        <VerdictScreen
          payload={verdict}
          onRetry={() => {
            setVerdict(null);
            setRoute("capture");
          }}
        />
      </>
    );
  }

  return (
    <>
      <StatusBar style="auto" />
      <CaptureScreen
        apiBaseUrl={API_BASE_URL}
        initialDraft={draft}
        onVerdict={(payload, sourceText) => {
          setDraft(sourceText);
          setVerdict(payload);
          setRoute("verdict");
        }}
        onShowHistory={() => setRoute("history")}
      />
    </>
  );
}

registerRootComponent(App);
