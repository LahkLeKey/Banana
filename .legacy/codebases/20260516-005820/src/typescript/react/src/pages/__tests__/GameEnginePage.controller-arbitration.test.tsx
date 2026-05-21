// @ts-nocheck -- bun:test types are not part of app tsconfig.

import { afterEach, beforeEach, describe, expect, test } from "bun:test";
import { act, cleanup, fireEvent, render, screen } from "@testing-library/react";

import { GameEnginePage } from "../GameEnginePage";

describe("GameEnginePage controller arbitration", () => {
  let originalBananaEngine: unknown;
  let originalSetInterval: typeof window.setInterval;
  let originalClearInterval: typeof window.clearInterval;
  let originalSetTimeout: typeof window.setTimeout;
  let originalClearTimeout: typeof window.clearTimeout;
  let fpsTick: (() => void) | null;
  let touchHoldTimer: (() => void) | null;
  const ccallCalls: Array<unknown[]> = [];

  beforeEach(() => {
    ccallCalls.length = 0;
    fpsTick = null;
    touchHoldTimer = null;

    originalBananaEngine = (window as any).BananaEngine;
    (window as any).BananaEngine = async () => ({
      ccall: (...args: unknown[]) => {
        ccallCalls.push(args);
        return 0;
      },
      cwrap: () => () => 0,
    });

    originalSetInterval = window.setInterval;
    originalClearInterval = window.clearInterval;
    originalSetTimeout = window.setTimeout;
    originalClearTimeout = window.clearTimeout;
    window.setInterval = ((cb: TimerHandler) => {
      fpsTick = cb as () => void;
      return 1;
    }) as typeof window.setInterval;
    window.clearInterval = (() => {
      return undefined;
    }) as typeof window.clearInterval;
    window.setTimeout = ((cb: TimerHandler) => {
      touchHoldTimer = cb as () => void;
      return 2;
    }) as typeof window.setTimeout;
    window.clearTimeout = (() => {
      return undefined;
    }) as typeof window.clearTimeout;
  });

  afterEach(() => {
    cleanup();
    (window as any).BananaEngine = originalBananaEngine;
    window.setInterval = originalSetInterval;
    window.clearInterval = originalClearInterval;
    window.setTimeout = originalSetTimeout;
    window.clearTimeout = originalClearTimeout;
  });

  async function renderReadyPage() {
    let view: ReturnType<typeof render> | undefined;
    await act(async () => {
      view = render(<GameEnginePage />);
      await Promise.resolve();
    });
    if (!view) {
      throw new Error("GameEnginePage did not render");
    }
    return view;
  }

  function expectLatestMoveInput(moveX: number, moveZ: number) {
    const moveCalls = ccallCalls.filter((args) => args[0] === "engine_set_move_input");
    expect(moveCalls.length).toBeGreaterThan(0);
    const last = moveCalls[moveCalls.length - 1] as unknown[];
    expect(last[3]).toEqual([moveX, moveZ]);
  }

  test("rapid right-click and keyboard alternation preserves movement authority", async () => {
    const { container } = await renderReadyPage();
    const canvas = container.querySelector("canvas") as HTMLCanvasElement;

    await act(async () => {
      fireEvent.contextMenu(canvas, { clientX: 140, clientY: 150 });
    });
    expect(screen.getByText("Actions")).not.toBeNull();

    await act(async () => {
      fireEvent.keyDown(window, { key: "w" });
    });
    expect(screen.queryByText("Actions")).toBeNull();

    await act(async () => {
      fpsTick?.();
    });
    expectLatestMoveInput(0, 1);

    await act(async () => {
      fireEvent.contextMenu(canvas, { clientX: 180, clientY: 190 });
    });
    expect(screen.getByText("Actions")).not.toBeNull();

    await act(async () => {
      fpsTick?.();
    });
    expectLatestMoveInput(0, 1);

    await act(async () => {
      fireEvent.keyUp(window, { key: "w" });
      fpsTick?.();
    });
    expectLatestMoveInput(0, 0);
  });

  test("blur clears active movement intent and sends neutral input", async () => {
    await renderReadyPage();

    await act(async () => {
      fireEvent.keyDown(window, { key: "d" });
      fpsTick?.();
    });
    expectLatestMoveInput(1, 0);

    await act(async () => {
      fireEvent.blur(window);
    });
    expectLatestMoveInput(0, 0);
  });

  test("runtime bridge emits stable engine ccall contract names", async () => {
    await renderReadyPage();

    await act(async () => {
      fireEvent.keyDown(window, { key: "w" });
      fpsTick?.();
    });

    const invokedNames = ccallCalls.map((args) => String(args[0]));
    expect(invokedNames.includes("engine_set_move_input")).toBeTrue();
    expect(invokedNames.includes("engine_tick")).toBeTrue();
  });

  test("action selection keeps movement loop responsive", async () => {
    const { container } = await renderReadyPage();
    const canvas = container.querySelector("canvas") as HTMLCanvasElement;

    await act(async () => {
      fireEvent.keyDown(window, { key: "d" });
      fpsTick?.();
    });
    expectLatestMoveInput(1, 0);

    await act(async () => {
      fireEvent.contextMenu(canvas, { clientX: 160, clientY: 180 });
    });
    expect(screen.getByText("Actions")).not.toBeNull();

    await act(async () => {
      fireEvent.click(screen.getByText("Interact"));
      fpsTick?.();
    });

    expect(screen.queryByText("Actions")).toBeNull();
    expectLatestMoveInput(1, 0);
  });

  test("mobile hold reveals the radial and maps touch direction to movement", async () => {
    const { container } = await renderReadyPage();
    const canvas = container.querySelector("canvas") as HTMLCanvasElement;

    await act(async () => {
      fireEvent.pointerDown(canvas, {
        pointerId: 7,
        pointerType: "touch",
        clientX: 200,
        clientY: 200,
      });
    });

    expect(screen.queryByTestId("mobile-radial-control")).toBeNull();

    await act(async () => {
      touchHoldTimer?.();
    });

    expect(screen.getByTestId("mobile-radial-control")).not.toBeNull();

    await act(async () => {
      fireEvent.pointerMove(canvas, {
        pointerId: 7,
        pointerType: "touch",
        clientX: 260,
        clientY: 200,
      });
      fpsTick?.();
    });

    expect(screen.getByTestId("mobile-radial-right").textContent).toBe("D");

    await act(async () => {
      fireEvent.pointerUp(canvas, {
        pointerId: 7,
        pointerType: "touch",
        clientX: 260,
        clientY: 200,
      });
      fpsTick?.();
    });

    expect(screen.queryByTestId("mobile-radial-control")).toBeNull();
  });
});
