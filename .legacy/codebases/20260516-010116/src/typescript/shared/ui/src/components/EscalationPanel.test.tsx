// Slice 023 -- EscalationPanel behavior coverage.
// Validates closed-by-default, expand-on-click, lazy load (called once),
// fingerprint render, and error fallback. The DOM is exercised via a
// minimal happy-dom-style window injected by bun:test through happy-dom.

// @ts-nocheck -- bun:test types are not available to the shared/ui
// tsconfig (the suite is consumed by `bun test`, not tsc). Component
// behavior is type-checked via the production export.
import { afterEach, beforeEach, describe, expect, mock, test } from "bun:test";
import { GlobalRegistrator } from "@happy-dom/global-registrator";
import { act } from "react";
import { createRoot, type Root } from "react-dom/client";

import { EscalationPanel } from "./EscalationPanel";

// Required by React 19 to silence act(...) warnings under bun:test.
(globalThis as unknown as { IS_REACT_ACT_ENVIRONMENT: boolean }).IS_REACT_ACT_ENVIRONMENT = true;

let container: HTMLDivElement;
let root: Root;

beforeEach(() => {
  if (!(globalThis as unknown as { document?: Document }).document) {
    GlobalRegistrator.register();
  }
  container = document.createElement("div");
  document.body.appendChild(container);
  root = createRoot(container);
});

afterEach(() => {
  act(() => {
    root.unmount();
  });
  container.remove();
});

function findByTestId(id: string): HTMLElement | null {
  return container.querySelector(`[data-testid="${id}"]`);
}

describe("EscalationPanel", () => {
  test("renders trigger and stays closed by default", () => {
    act(() => {
      root.render(<EscalationPanel loadSummary={async () => ({ embedding: null })} />);
    });

    const trigger = findByTestId("escalation-panel-trigger");
    expect(trigger).not.toBeNull();
    expect(trigger?.getAttribute("aria-expanded")).toBe("false");
    expect(findByTestId("escalation-panel-body")).toBeNull();
  });

  test("expands on click and renders the 4-component fingerprint", async () => {
    const summary = { embedding: [0.42, -0.11, 0.07, 0.83] };
    const loader = mock(async () => summary);

    act(() => {
      root.render(<EscalationPanel loadSummary={loader} />);
    });

    const trigger = findByTestId("escalation-panel-trigger")!;
    await act(async () => {
      trigger.click();
    });
    // resolve the loader microtask
    await act(async () => {
      await Promise.resolve();
    });

    expect(findByTestId("escalation-panel-body")).not.toBeNull();
    const fingerprint = findByTestId("escalation-panel-fingerprint");
    expect(fingerprint).not.toBeNull();
    const rows = fingerprint!.querySelectorAll("tr");
    expect(rows.length).toBe(4);
    expect(rows[0].textContent).toContain("banana_context");
    expect(rows[3].textContent).toContain("banana_probability");
    expect(rows[0].textContent).toContain("0.4200");
  });

  test("only loads the summary once across multiple expands", async () => {
    const loader = mock(async () => ({ embedding: [1, 2, 3, 4] }));

    act(() => {
      root.render(<EscalationPanel loadSummary={loader} />);
    });

    const trigger = findByTestId("escalation-panel-trigger")!;
    await act(async () => {
      trigger.click();
    });
    await act(async () => {
      await Promise.resolve();
    });
    await act(async () => {
      trigger.click();
    }); // close
    await act(async () => {
      trigger.click();
    }); // re-open
    await act(async () => {
      await Promise.resolve();
    });

    expect(loader).toHaveBeenCalledTimes(1);
  });

  test("renders error fallback when loader throws", async () => {
    const loader = mock(async () => {
      throw new Error("Couldn\u2019t reach the banana service. Try again.");
    });

    act(() => {
      root.render(<EscalationPanel loadSummary={loader} />);
    });

    const trigger = findByTestId("escalation-panel-trigger")!;
    await act(async () => {
      trigger.click();
    });
    await act(async () => {
      await Promise.resolve();
    });

    const error = findByTestId("escalation-panel-error");
    expect(error).not.toBeNull();
    expect(error?.textContent).toContain("Try again");
  });

  test("renders empty fallback when embedding is null", async () => {
    act(() => {
      root.render(<EscalationPanel loadSummary={async () => ({ embedding: null })} />);
    });

    const trigger = findByTestId("escalation-panel-trigger")!;
    await act(async () => {
      trigger.click();
    });
    await act(async () => {
      await Promise.resolve();
    });

    expect(findByTestId("escalation-panel-empty")).not.toBeNull();
  });
});
