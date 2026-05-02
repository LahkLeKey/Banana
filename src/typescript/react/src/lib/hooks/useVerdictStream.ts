/**
 * useVerdictStream.ts — React hook for SSE verdict streaming (spec #054).
 *
 * const { events, isStreaming, verdict } = useVerdictStream(text);
 *
 * - Opens an EventSource to GET /api/v1/streaming/verdict?text=<text>
 * - Accumulates StreamEvent[] as SSE messages arrive
 * - Sets verdict when a "complete" event is received
 * - Cleans up the EventSource on unmount or when text changes
 */
import {useEffect, useRef, useState} from 'react';

const API_BASE =
  (import.meta.env.VITE_BANANA_API_BASE_URL as string | undefined) ??
  'http://localhost:8081';

export interface ProgressEvent {
  type: 'progress';
  step: string;
  pct: number;
}

export interface CompleteEvent {
  type: 'complete';
  verdict: VerdictResult;
}

export type StreamEvent = ProgressEvent | CompleteEvent;

export interface VerdictResult {
  label: string;
  score: number;
  status: string;
  [key: string]: unknown;
}

export function useVerdictStream(text: string | null): {
  events: StreamEvent[];
  isStreaming: boolean;
  verdict: VerdictResult | null;
} {
  const [events, setEvents] = useState<StreamEvent[]>([]);
  const [isStreaming, setIsStreaming] = useState(false);
  const [verdict, setVerdict] = useState<VerdictResult | null>(null);
  const esRef = useRef<EventSource | null>(null);

  useEffect(() => {
    if (!text) return;

    // Reset state for new stream
    setEvents([]);
    setVerdict(null);
    setIsStreaming(true);

    const url = `${API_BASE}/api/v1/streaming/verdict?text=${encodeURIComponent(text)}`;
    const es = new EventSource(url);
    esRef.current = es;

    es.onmessage = (e: MessageEvent) => {
      try {
        const parsed = JSON.parse(e.data as string) as StreamEvent;
        setEvents((prev) => [...prev, parsed]);
        if (parsed.type === 'complete') {
          setVerdict(parsed.verdict);
          setIsStreaming(false);
          es.close();
        }
      } catch {
        // malformed event — ignore
      }
    };

    es.onerror = () => {
      setIsStreaming(false);
      es.close();
    };

    return () => {
      es.close();
      esRef.current = null;
    };
  }, [text]);

  return {events, isStreaming, verdict};
}
