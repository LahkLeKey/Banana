// @ts-nocheck -- bun:test types not in app tsconfig
import { describe, expect, test } from "bun:test";

import {
  initialSubmitState,
  type SubmitAction,
  type SubmitState,
  submitReducer,
} from "./submitMachine";

type Verdict = {
  label: string;
};

function s(state: SubmitState<Verdict>): SubmitState<Verdict> {
  return state;
}

describe("submitReducer (spec 048)", () => {
  test("starts in idle with null lastResult", () => {
    const init = initialSubmitState<Verdict>();
    expect(init.kind).toBe("idle");
    expect((init as Extract<SubmitState<Verdict>, { kind: "idle" }>).lastResult).toBeNull();
  });

  test("idle -> submit -> submitting carries requestId and null lastResult", () => {
    const next = submitReducer(initialSubmitState<Verdict>(), {
      type: "submit",
      requestId: "r1",
    } satisfies SubmitAction<Verdict>);
    expect(next.kind).toBe("submitting");
    if (next.kind === "submitting") {
      expect(next.requestId).toBe("r1");
      expect(next.lastResult).toBeNull();
    }
  });

  test("submitting -> resolve (matching id) -> success", () => {
    const submitting = submitReducer(initialSubmitState<Verdict>(), {
      type: "submit",
      requestId: "r1",
    });
    const next = submitReducer(submitting, {
      type: "resolve",
      requestId: "r1",
      result: { label: "banana" },
    });
    expect(next.kind).toBe("success");
    if (next.kind === "success") {
      expect(next.result.label).toBe("banana");
      expect(next.lastResult.label).toBe("banana");
    }
  });

  test("submitting -> reject (matching id) -> error preserves lastResult", () => {
    let state: SubmitState<Verdict> = initialSubmitState();
    state = submitReducer(state, { type: "submit", requestId: "r1" });
    state = submitReducer(state, { type: "resolve", requestId: "r1", result: { label: "banana" } });
    state = submitReducer(state, { type: "submit", requestId: "r2" });
    state = submitReducer(state, { type: "reject", requestId: "r2", message: "boom" });
    expect(state.kind).toBe("error");
    if (state.kind === "error") {
      expect(state.message).toBe("boom");
      expect(state.lastResult?.label).toBe("banana");
    }
  });

  test("stale resolve is ignored (non-matching requestId)", () => {
    let state: SubmitState<Verdict> = initialSubmitState();
    state = submitReducer(state, { type: "submit", requestId: "r1" });
    const stale = submitReducer(state, {
      type: "resolve",
      requestId: "OLD",
      result: { label: "x" },
    });
    expect(stale).toBe(state);
  });

  test("stale reject is ignored (non-matching requestId)", () => {
    let state: SubmitState<Verdict> = initialSubmitState();
    state = submitReducer(state, { type: "submit", requestId: "r1" });
    const stale = submitReducer(state, { type: "reject", requestId: "OLD", message: "boom" });
    expect(stale).toBe(state);
  });

  test("submit while submitting is ignored (idempotency)", () => {
    let state: SubmitState<Verdict> = initialSubmitState();
    state = submitReducer(state, { type: "submit", requestId: "r1" });
    const second = submitReducer(state, { type: "submit", requestId: "r2" });
    expect(second).toBe(state);
  });

  test("success -> submit carries lastResult forward", () => {
    let state: SubmitState<Verdict> = initialSubmitState();
    state = submitReducer(state, { type: "submit", requestId: "r1" });
    state = submitReducer(state, { type: "resolve", requestId: "r1", result: { label: "banana" } });
    state = submitReducer(state, { type: "submit", requestId: "r2" });
    expect(state.kind).toBe("submitting");
    if (state.kind === "submitting") {
      expect(state.lastResult?.label).toBe("banana");
    }
  });

  test("reset returns idle with null lastResult", () => {
    let state: SubmitState<Verdict> = initialSubmitState();
    state = submitReducer(state, { type: "submit", requestId: "r1" });
    state = submitReducer(state, { type: "resolve", requestId: "r1", result: { label: "banana" } });
    const reset = submitReducer(state, { type: "reset" });
    expect(reset.kind).toBe("idle");
    if (reset.kind === "idle") {
      expect(reset.lastResult).toBeNull();
    }
  });
});

// Suppress unused warning for helper
void s;
