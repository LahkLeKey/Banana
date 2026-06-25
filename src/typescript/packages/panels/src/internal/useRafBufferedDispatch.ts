import {useCallback, useEffect, useMemo, useRef} from 'react';

export type RafBufferedDispatch<T> = {
  readonly pendingRef: React.MutableRefObject<T|null>; readonly schedule: (payload: T) => void; readonly flush: (
                                                                                                             commitPending?:
                                                                                                                 boolean) =>
                                                                                                             void;
};

export function useRafBufferedDispatch<T>(dispatch?: (payload: T) => void):
    RafBufferedDispatch<T> {
  const rafRef = useRef<number|null>(null);
  const pendingRef = useRef<T|null>(null);

  const schedule = useCallback((payload: T) => {
    pendingRef.current = payload;
    if (rafRef.current !== null) {
      return;
    }

    rafRef.current = window.requestAnimationFrame(() => {
      rafRef.current = null;
      const nextPayload = pendingRef.current;
      if (nextPayload === null) {
        return;
      }

      pendingRef.current = null;
      dispatch?.(nextPayload);
    });
  }, [dispatch]);

  const flush = useCallback((commitPending: boolean = false) => {
    if (rafRef.current !== null) {
      window.cancelAnimationFrame(rafRef.current);
      rafRef.current = null;
    }

    const payload = pendingRef.current;
    pendingRef.current = null;
    if (commitPending && payload !== null) {
      dispatch?.(payload);
    }
  }, [dispatch]);

  useEffect(() => {
    return () => {
      if (rafRef.current !== null) {
        window.cancelAnimationFrame(rafRef.current);
        rafRef.current = null;
      }
      pendingRef.current = null;
    };
  }, []);

  return useMemo(() => ({pendingRef, schedule, flush}), [schedule, flush]);
}
