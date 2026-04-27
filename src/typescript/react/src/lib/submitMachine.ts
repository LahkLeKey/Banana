import {useCallback, useReducer, useRef} from 'react';

export type SubmitState<TResult> =|{
  kind: 'idle';
  lastResult: TResult|null
}
|{
  kind: 'submitting';
  requestId: string;
  lastResult: TResult|null
}
|{
  kind: 'success';
  result: TResult;
  lastResult: TResult
}
|{
  kind: 'error';
  message: string;
  lastResult: TResult|null
};

export type SubmitAction<TResult> =|{
  type: 'submit';
  requestId: string
}
|{
  type: 'resolve';
  requestId: string;
  result: TResult
}
|{
  type: 'reject';
  requestId: string;
  message: string
}
|{type: 'reset'};

export function submitReducer<TResult>(
    state: SubmitState<TResult>,
    action: SubmitAction<TResult>,
    ): SubmitState<TResult> {
  switch (action.type) {
    case 'submit': {
      if (state.kind === 'submitting') {
        // Idempotency guard: ignore additional submits while in flight.
        return state;
      }
      const lastResult =
          state.kind === 'success' ? state.result : state.lastResult;
      return {kind: 'submitting', requestId: action.requestId, lastResult};
    }
    case 'resolve': {
      if (state.kind !== 'submitting' || state.requestId !== action.requestId) {
        return state;
      }
      return {
        kind: 'success',
        result: action.result,
        lastResult: action.result,
      };
    }
    case 'reject': {
      if (state.kind !== 'submitting' || state.requestId !== action.requestId) {
        return state;
      }
      return {
        kind: 'error',
        message: action.message,
        lastResult: state.lastResult,
      };
    }
    case 'reset': {
      return {kind: 'idle', lastResult: null};
    }
    default: {
      const _exhaustive: never = action;
      return state;
    }
  }
}

export function initialSubmitState<TResult>(): SubmitState<TResult> {
  return {kind: 'idle', lastResult: null};
}

let counter = 0;

function mintRequestId(): string {
  counter += 1;
  return `req_${Date.now().toString(36)}_${counter.toString(36)}`;
}

export interface UseSubmitMachine<TResult> {
  state: SubmitState<TResult>;
  submit: (run: () => Promise<TResult>) => Promise<void>;
  reset: () => void;
}

export function useSubmitMachine<TResult>(): UseSubmitMachine<TResult> {
  const [state, dispatch] = useReducer(
      submitReducer as (
          s: SubmitState<TResult>,
          a: SubmitAction<TResult>,
          ) => SubmitState<TResult>,
      undefined,
      initialSubmitState<TResult>,
  );
  const inFlight = useRef<string|null>(null);

  const submit = useCallback(async (run: () => Promise<TResult>) => {
    if (inFlight.current !== null) {
      // Idempotency guard at hook layer.
      return;
    }
    const requestId = mintRequestId();
    inFlight.current = requestId;
    dispatch({type: 'submit', requestId});
    try {
      const result = await run();
      dispatch({type: 'resolve', requestId, result});
    } catch (error: unknown) {
      const message = error instanceof Error ? error.message : 'Submit failed';
      dispatch({type: 'reject', requestId, message});
    } finally {
      if (inFlight.current === requestId) {
        inFlight.current = null;
      }
    }
  }, []);

  const reset = useCallback(() => {
    inFlight.current = null;
    dispatch({type: 'reset'});
  }, []);

  return {state, submit, reset};
}
