// @ts-nocheck -- bun:test types not in app tsconfig; behavior asserted via DOM.
//
// Spec 261 T011: Input routing transition contract tests.
// Validates viewport mode vs overlay mode transitions and focus-lock behavior.

import { afterEach, describe, expect, test } from "bun:test";
import { cleanup, render } from "@testing-library/react";

import { OverlayStack } from "../overlays/OverlayStack";

afterEach(cleanup);

// ── T011 Input routing contract tests ────────────────────────────────────────

describe("OverlayStack focus-lock / overlayMode transitions", () => {
  test("overlayMode is false when no focus-lock layers are provided", () => {
    let reported: boolean | undefined;

    render(
      <OverlayStack
        hud={<div>hud</div>}
        debug={<div>debug</div>}
        onOverlayModeChange={(active) => {
          reported = active;
        }}
      />
    );

    expect(reported).toBe(false);
  });

  test("overlayMode is true when menu layer is provided", () => {
    let reported: boolean | undefined;

    render(
      <OverlayStack
        menu={<div data-testid="menu">menu</div>}
        onOverlayModeChange={(active) => {
          reported = active;
        }}
      />
    );

    expect(reported).toBe(true);
  });

  test("overlayMode is true when modal layer is provided", () => {
    let reported: boolean | undefined;

    render(
      <OverlayStack
        modal={<div data-testid="modal">modal</div>}
        onOverlayModeChange={(active) => {
          reported = active;
        }}
      />
    );

    expect(reported).toBe(true);
  });

  test("overlayMode is false when only hud and debug layers are provided", () => {
    let reported: boolean | undefined;

    render(
      <OverlayStack
        hud={<div>hud</div>}
        debug={<div>fps</div>}
        onOverlayModeChange={(active) => {
          reported = active;
        }}
      />
    );

    expect(reported).toBe(false);
  });

  test("overlay layers render at correct z-index slots", () => {
    const { container } = render(
      <OverlayStack
        hud={<div data-testid="hud-child">hud</div>}
        menu={<div data-testid="menu-child">menu</div>}
        modal={<div data-testid="modal-child">modal</div>}
        debug={<div data-testid="debug-child">debug</div>}
      />
    );

    // Each slot is a fixed-position wrapper div; find all of them by style
    const slots = Array.from(container.querySelectorAll("div")).filter(
      (el) => el.style.position === "fixed"
    );
    expect(slots.length).toBe(4);

    // z-index order: hud=10, menu=20, modal=30, debug=40
    const zIndexes = slots.map((el) => Number(el.style.zIndex));
    expect(zIndexes).toEqual([10, 20, 30, 40]);
  });

  test("overlay slots have pointer-events: none by default", () => {
    const { container } = render(<OverlayStack hud={<div>hud</div>} menu={<div>menu</div>} />);

    const slots = container.querySelectorAll(":scope > div");
    for (const slot of slots) {
      expect((slot as HTMLElement).style.pointerEvents).toBe("none");
    }
  });
});

// ── WasmErrorBanner interactive overlay tests ─────────────────────────────────

describe("WasmErrorBanner interaction contract", () => {
  test("renders nothing when message is null", () => {
    const { container } = render(
      // Import inline to keep test self-contained
      require("../wasm/WasmErrorBanner").WasmErrorBanner({ message: null })
    );
    // jsx returns null → container is empty
    expect(container.innerHTML).toBe("");
  });
});
