// @ts-nocheck -- bun:test types are not part of app tsconfig.

import { afterEach, beforeEach, describe, expect, test } from "bun:test";
import { act, cleanup, fireEvent, render, screen } from "@testing-library/react";

import { computeContextMenuPosition, GameEnginePage } from "../GameEnginePage";

describe("GameEnginePage context menu geometry", () => {
    test("clamps menu position within viewport bounds", () => {
        const pos = computeContextMenuPosition(1200, 900, 1280, 720);
        expect(pos.x).toBe(1052);
        expect(pos.y).toBe(572);
    });

    test("keeps top-left minimum padding", () => {
        const pos = computeContextMenuPosition(2, 3, 1280, 720);
        expect(pos.x).toBe(8);
        expect(pos.y).toBe(8);
    });
});

describe("GameEnginePage context menu behavior", () => {
    let originalBananaEngine: unknown;

    beforeEach(() => {
        originalBananaEngine = (window as any).BananaEngine;
        (window as any).BananaEngine = async () => ({
            ccall: () => 0,
            cwrap: () => () => 0,
        });
    });

    afterEach(() => {
        cleanup();
        (window as any).BananaEngine = originalBananaEngine;
    });

    async function renderReadyPage() {
        let view: ReturnType<typeof render>;
        await act(async () => {
            view = render(<GameEnginePage />);
            await Promise.resolve();
        });
        const script = document.querySelector('script[src*="/wasm/engine.js"]') as HTMLScriptElement | null;
        if (script?.onload) {
            await act(async () => {
                script.onload?.(new Event("load"));
            });
        }
        return view!;
    }

    test("opens on right-click and closes on outside mouse down", async () => {
        const { container } = await renderReadyPage();
        const canvas = container.querySelector("canvas") as HTMLCanvasElement;

        await act(async () => {
            fireEvent.contextMenu(canvas, { clientX: 100, clientY: 120 });
        });

        expect(screen.getByText("Actions")).not.toBeNull();

        await act(async () => {
            fireEvent.mouseDown(canvas);
        });

        expect(screen.queryByText("Actions")).toBeNull();
    });

    test("closes on Escape", async () => {
        const { container } = await renderReadyPage();
        const canvas = container.querySelector("canvas") as HTMLCanvasElement;

        await act(async () => {
            fireEvent.contextMenu(canvas, { clientX: 120, clientY: 140 });
        });

        expect(screen.getByText("Actions")).not.toBeNull();

        await act(async () => {
            fireEvent.keyDown(window, { key: "Escape" });
        });

        expect(screen.queryByText("Actions")).toBeNull();
    });
});
