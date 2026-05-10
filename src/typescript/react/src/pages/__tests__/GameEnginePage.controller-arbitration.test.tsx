// @ts-nocheck -- bun:test types are not part of app tsconfig.

import { afterEach, beforeEach, describe, expect, test } from "bun:test";
import { act, cleanup, fireEvent, render, screen } from "@testing-library/react";

import { GameEnginePage } from "../GameEnginePage";

describe("GameEnginePage controller arbitration", () => {
  let originalBananaEngine: unknown;
  let originalSetInterval: typeof window.setInterval;
  let originalClearInterval: typeof window.clearInterval;
  let fpsTick: (() => void) | null;
  const ccallCalls: Array<unknown[]> = [];

  beforeEach(() => {
    ccallCalls.length = 0;
    fpsTick = null;

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
    window.setInterval = ((cb: TimerHandler) => {
      fpsTick = cb as () => void;
      return 1;
    }) as typeof window.setInterval;
    window.clearInterval = (() => {
      return undefined;
    }) as typeof window.clearInterval;
  });

  afterEach(() => {
    cleanup();
    (window as any).BananaEngine = originalBananaEngine;
    window.setInterval = originalSetInterval;
    window.clearInterval = originalClearInterval;
  });

  async function renderReadyPage() {
    let view: ReturnType<typeof render>;
    await act(async () => {
      view = render(<GameEnginePage />);
      await Promise.resolve();
    });
    return view!;
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
});
