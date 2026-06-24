// Slice 036 - ResizablePanel behavior and coverage suite.
// @ts-nocheck
import { afterEach, beforeEach, describe, expect, mock, test } from "bun:test";
import { GlobalRegistrator } from "@happy-dom/global-registrator";
import { act } from "react";
import { createRoot, type Root } from "react-dom/client";

import { ResizablePanel } from "./ResizablePanel";

(globalThis as unknown as { IS_REACT_ACT_ENVIRONMENT: boolean }).IS_REACT_ACT_ENVIRONMENT = true;

let container: HTMLDivElement;
let root: Root;
let originalRaf: typeof window.requestAnimationFrame;
let originalCancelRaf: typeof window.cancelAnimationFrame;

function dispatchMouseMove(x: number, y: number): void {
    document.dispatchEvent(new MouseEvent("mousemove", { bubbles: true, clientX: x, clientY: y }));
}

function dispatchMouseUp(x: number, y: number): void {
    document.dispatchEvent(new MouseEvent("mouseup", { bubbles: true, clientX: x, clientY: y }));
}

function mouseDown(node: Element, x: number, y: number): void {
    node.dispatchEvent(new MouseEvent("mousedown", { bubbles: true, cancelable: true, clientX: x, clientY: y }));
}

beforeEach(() => {
    if (!(globalThis as unknown as { document?: Document }).document) {
        GlobalRegistrator.register();
    }

    originalRaf = window.requestAnimationFrame;
    originalCancelRaf = window.cancelAnimationFrame;
    window.requestAnimationFrame = ((cb: FrameRequestCallback) => {
        cb(0);
        return 1;
    }) as typeof window.requestAnimationFrame;
    window.cancelAnimationFrame = (() => { }) as typeof window.cancelAnimationFrame;

    container = document.createElement("div");
    document.body.appendChild(container);
    root = createRoot(container);
});

afterEach(() => {
    act(() => {
        root.unmount();
    });
    container.remove();
    window.requestAnimationFrame = originalRaf;
    window.cancelAnimationFrame = originalCancelRaf;
});

describe("ResizablePanel", () => {
    test("renders inside a single host container with behavior pipeline metadata", () => {
        act(() => {
            root.render(
                <ResizablePanel
                    id="hud"
                    title="HUD"
                    x={100}
                    y={80}
                    width={320}
                    height={260}
                    onMove={() => { }}
                    onResize={() => { }}
                    onAnchorCommit={() => { }}
                >
                    content
                </ResizablePanel>,
            );
        });

        const host = container.querySelector('[data-resizable-panel-host="true"]') as HTMLDivElement;
        const panel = container.querySelector('[data-resizable-panel="true"]') as HTMLElement;

        expect(host).not.toBeNull();
        expect(panel).not.toBeNull();
        expect(panel.dataset.panelBehaviorPipeline).toBe("base>movable>resizable>dockable");
        expect(host.children.length).toBeGreaterThan(0);
    });

    test("supports stage element and stage props overrides", () => {
        act(() => {
            root.render(
                <ResizablePanel
                    id="analysis"
                    title="Analysis"
                    x={40}
                    y={20}
                    width={300}
                    height={220}
                    stageElements={{ container: "section", header: "header", content: "article" }}
                    stageElementProps={{
                        container: { "aria-label": "analysis panel", className: "custom-container" },
                        header: { className: "custom-header" },
                        content: { className: "custom-content" },
                    }}
                >
                    panel body
                </ResizablePanel>,
            );
        });

        const panel = container.querySelector('[data-resizable-panel="true"]') as HTMLElement;
        const header = panel.querySelector(".custom-header") as HTMLElement;
        const content = panel.querySelector(".custom-content") as HTMLElement;

        expect(panel.tagName).toBe("SECTION");
        expect(panel.className).toContain("custom-container");
        expect(panel.getAttribute("aria-label")).toBe("analysis panel");
        expect(header.tagName).toBe("HEADER");
        expect(content.tagName).toBe("ARTICLE");
    });

    test("collapsed mode behaves like a keyboard-accessible launcher", () => {
        const onExpand = mock(() => { });

        act(() => {
            root.render(
                <ResizablePanel
                    id="inventory"
                    x={10}
                    y={20}
                    width={280}
                    height={220}
                    isCollapsed
                    onExpand={onExpand}
                >
                    hidden
                </ResizablePanel>,
            );
        });

        const launcher = container.querySelector('[role="button"]') as HTMLDivElement;
        expect(launcher).not.toBeNull();

        act(() => {
            launcher.dispatchEvent(new KeyboardEvent("keydown", { bubbles: true, key: "Enter" }));
            launcher.dispatchEvent(new KeyboardEvent("keydown", { bubbles: true, key: " " }));
            launcher.click();
        });

        expect(onExpand).toHaveBeenCalledTimes(3);
    });

    test("fires control button callbacks and title labeling", () => {
        const onUnlinkAll = mock(() => { });
        const onToggleGroupResizeLock = mock(() => { });
        const onExpand = mock(() => { });
        const onCollapse = mock(() => { });

        act(() => {
            root.render(
                <ResizablePanel
                    id="quests"
                    title="Quests"
                    titleElementId="quests-title"
                    x={80}
                    y={60}
                    width={320}
                    height={240}
                    isAnchored
                    groupSize={3}
                    isGroupResizeLocked={false}
                    onUnlinkAll={onUnlinkAll}
                    onToggleGroupResizeLock={onToggleGroupResizeLock}
                    onExpand={onExpand}
                    onCollapse={onCollapse}
                >
                    items
                </ResizablePanel>,
            );
        });

        const title = container.querySelector("#quests-title") as HTMLElement;
        expect(title.textContent).toContain("Quests");

        const unlink = container.querySelector('button[title^="Unlink"]') as HTMLButtonElement;
        const lock = container.querySelector('button[title="Lock group resize"]') as HTMLButtonElement;
        const expand = container.querySelector('button[title="Expand panel"]') as HTMLButtonElement;
        const collapse = container.querySelector('button[title="Collapse panel"]') as HTMLButtonElement;

        act(() => {
            unlink.click();
            lock.click();
            expand.click();
            collapse.click();
        });

        expect(onUnlinkAll).toHaveBeenCalledTimes(1);
        expect(onToggleGroupResizeLock).toHaveBeenCalledTimes(1);
        expect(onExpand).toHaveBeenCalledTimes(1);
        expect(onCollapse).toHaveBeenCalledTimes(1);
    });

    test("moves panel and commits anchor when snapping to a nearby panel", async () => {
        const onMove = mock(() => { });
        const onAnchorCommit = mock(() => { });

        act(() => {
            root.render(
                <ResizablePanel
                    id="panel-a"
                    title="Panel A"
                    x={100}
                    y={100}
                    width={300}
                    height={220}
                    onMove={onMove}
                    onAnchorCommit={onAnchorCommit}
                >
                    A
                </ResizablePanel>,
            );
        });

        const panel = container.querySelector('[data-panel-id="panel-a"]') as HTMLElement;
        const header = Array.from(panel.children).find(
            (node) => (node as HTMLElement).style.cursor === "grab",
        ) as HTMLElement;

        Object.defineProperty(panel, "getBoundingClientRect", {
            configurable: true,
            value: () => ({
                x: 100,
                y: 100,
                left: 100,
                top: 100,
                width: 300,
                height: 220,
                right: 400,
                bottom: 320,
                toJSON: () => ({}),
            }),
        });

        const target = document.createElement("div");
        target.dataset.resizablePanel = "true";
        target.dataset.panelId = "panel-b";
        Object.defineProperty(target, "getBoundingClientRect", {
            configurable: true,
            value: () => ({
                x: 404,
                y: 100,
                left: 404,
                top: 100,
                width: 260,
                height: 220,
                right: 664,
                bottom: 320,
                toJSON: () => ({}),
            }),
        });
        document.body.appendChild(target);

        act(() => {
            mouseDown(header, 120, 120);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(140, 120);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseUp(140, 120);
        });

        expect(onMove).toHaveBeenCalled();
        expect(onMove.mock.calls.at(-1)?.[2]).toBe("end");
        expect(onAnchorCommit).toHaveBeenCalledWith("panel-b", "left");

        target.remove();
    });

    test("does not commit anchor when dragging grouped panels", async () => {
        const onMove = mock(() => { });
        const onAnchorCommit = mock(() => { });

        act(() => {
            root.render(
                <ResizablePanel
                    id="panel-group"
                    title="Grouped"
                    x={100}
                    y={100}
                    width={300}
                    height={220}
                    groupSize={2}
                    onMove={onMove}
                    onAnchorCommit={onAnchorCommit}
                >
                    group
                </ResizablePanel>,
            );
        });

        const panel = container.querySelector('[data-panel-id="panel-group"]') as HTMLElement;
        const header = Array.from(panel.children).find(
            (node) => (node as HTMLElement).style.cursor === "grab",
        ) as HTMLElement;

        Object.defineProperty(panel, "getBoundingClientRect", {
            configurable: true,
            value: () => ({
                x: 100,
                y: 100,
                left: 100,
                top: 100,
                width: 300,
                height: 220,
                right: 400,
                bottom: 320,
                toJSON: () => ({}),
            }),
        });

        act(() => {
            mouseDown(header, 120, 120);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(180, 180);
            dispatchMouseUp(180, 180);
        });

        expect(onMove).toHaveBeenCalled();
        expect(onAnchorCommit).toHaveBeenCalledTimes(0);
    });

    test("resizes through all handle directions", async () => {
        const onResize = mock(() => { });

        act(() => {
            root.render(
                <ResizablePanel
                    id="panel-resize"
                    title="Resize"
                    x={120}
                    y={90}
                    width={320}
                    height={240}
                    minWidth={200}
                    minHeight={150}
                    onResize={onResize}
                >
                    content
                </ResizablePanel>,
            );
        });

        const panel = container.querySelector('[data-panel-id="panel-resize"]') as HTMLElement;

        Object.defineProperty(panel, "getBoundingClientRect", {
            configurable: true,
            value: () => ({
                x: 120,
                y: 90,
                left: 120,
                top: 90,
                width: 320,
                height: 240,
                right: 440,
                bottom: 330,
                toJSON: () => ({}),
            }),
        });

        const handles = Array.from(panel.children).filter(
            (node) => (node as HTMLElement).style.background === "transparent",
        ) as HTMLElement[];
        const left = handles.find((node) => node.style.cursor === "ew-resize" && node.style.left === "0px") as HTMLElement;
        const right = handles.find((node) => node.style.cursor === "ew-resize" && node.style.right === "0px") as HTMLElement;
        const top = handles.find((node) => node.style.cursor === "ns-resize" && node.style.top === "0px") as HTMLElement;
        const bottom = handles.find((node) => node.style.cursor === "ns-resize" && node.style.bottom === "0px") as HTMLElement;
        const corner = panel.querySelector('[title="Resize panel"]') as HTMLElement;

        expect(left).not.toBeUndefined();
        expect(right).not.toBeUndefined();
        expect(top).not.toBeUndefined();
        expect(bottom).not.toBeUndefined();
        expect(corner).not.toBeNull();

        act(() => {
            mouseDown(left, 120, 180);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(90, 180);
            dispatchMouseUp(90, 180);

            mouseDown(right, 440, 180);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(480, 180);
            dispatchMouseUp(480, 180);

            mouseDown(top, 180, 90);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(180, 70);
            dispatchMouseUp(180, 70);

            mouseDown(bottom, 180, 330);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(180, 360);
            dispatchMouseUp(180, 360);

            mouseDown(corner, 440, 330);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(500, 390);
            dispatchMouseUp(500, 390);
        });

        expect(onResize).toHaveBeenCalled();
        const lastPayload = onResize.mock.calls.at(-1)?.[1];
        expect(lastPayload.width).toBeGreaterThanOrEqual(200);
        expect(lastPayload.height).toBeGreaterThanOrEqual(150);
    });

    test("covers deferred RAF buffering for move and resize pipelines", async () => {
        const onMove = mock(() => { });
        const onResize = mock(() => { });
        const rafCallbacks: FrameRequestCallback[] = [];

        window.requestAnimationFrame = ((cb: FrameRequestCallback) => {
            rafCallbacks.push(cb);
            return rafCallbacks.length;
        }) as typeof window.requestAnimationFrame;
        window.cancelAnimationFrame = (() => { }) as typeof window.cancelAnimationFrame;

        act(() => {
            root.render(
                <ResizablePanel
                    id="panel-buffer"
                    title="Buffer"
                    x={100}
                    y={100}
                    width={320}
                    height={240}
                    onMove={onMove}
                    onResize={onResize}
                >
                    buffered
                </ResizablePanel>,
            );
        });

        const panel = container.querySelector('[data-panel-id="panel-buffer"]') as HTMLElement;
        Object.defineProperty(panel, "getBoundingClientRect", {
            configurable: true,
            value: () => ({
                x: 100,
                y: 100,
                left: 100,
                top: 100,
                width: 320,
                height: 240,
                right: 420,
                bottom: 340,
                toJSON: () => ({}),
            }),
        });

        const header = Array.from(panel.children).find(
            (node) => (node as HTMLElement).style.cursor === "grab",
        ) as HTMLElement;

        const handles = Array.from(panel.children).filter(
            (node) => (node as HTMLElement).style.background === "transparent",
        ) as HTMLElement[];
        const right = handles.find((node) => node.style.cursor === "ew-resize" && node.style.right === "0px") as HTMLElement;

        act(() => {
            mouseDown(header, 120, 120);
        });
        await Promise.resolve();

        act(() => {
            dispatchMouseMove(150, 120);
            dispatchMouseMove(180, 120);
            dispatchMouseUp(180, 120);
        });

        act(() => {
            mouseDown(right, 420, 160);
        });
        await Promise.resolve();

        act(() => {
            dispatchMouseMove(460, 160);
            dispatchMouseMove(500, 160);
            dispatchMouseUp(500, 160);
        });

        // Execute deferred callbacks after flush paths cleared pending refs.
        act(() => {
            for (const cb of rafCallbacks) {
                cb(0);
            }
        });

        expect(onMove).toHaveBeenCalled();
        expect(onResize).toHaveBeenCalled();
    });

    test("renders side badges and takes tiny-movement drag branch", async () => {
        const onMove = mock(() => { });

        act(() => {
            root.render(
                <ResizablePanel
                    id="panel-badges"
                    title="Badges"
                    x={100}
                    y={100}
                    width={320}
                    height={240}
                    onMove={onMove}
                    anchorSides={["left", "top"]}
                    groupColor="#22d3ee"
                >
                    badges
                </ResizablePanel>,
            );
        });

        const panel = container.querySelector('[data-panel-id="panel-badges"]') as HTMLElement;
        Object.defineProperty(panel, "getBoundingClientRect", {
            configurable: true,
            value: () => ({
                x: 100,
                y: 100,
                left: 100,
                top: 100,
                width: 320,
                height: 240,
                right: 420,
                bottom: 340,
                toJSON: () => ({}),
            }),
        });

        const badgeNodes = Array.from(panel.querySelectorAll("span")).filter(
            (node) => (node as HTMLElement).textContent === "l" || (node as HTMLElement).textContent === "t",
        );
        expect(badgeNodes.length).toBe(2);

        const header = Array.from(panel.children).find(
            (node) => (node as HTMLElement).style.cursor === "grab",
        ) as HTMLElement;

        act(() => {
            mouseDown(header, 120, 120);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(121, 121);
            dispatchMouseUp(121, 121);
        });

        expect(onMove).toHaveBeenCalled();
    });

    test("guards header drag when pressing a button and handles null rect guards", async () => {
        const onExpand = mock(() => { });
        const onResize = mock(() => { });

        act(() => {
            root.render(
                <ResizablePanel
                    id="panel-guard"
                    title="Guard"
                    x={100}
                    y={100}
                    width={320}
                    height={240}
                    onExpand={onExpand}
                    onResize={onResize}
                >
                    guard
                </ResizablePanel>,
            );
        });

        const panel = container.querySelector('[data-panel-id="panel-guard"]') as HTMLElement;
        Object.defineProperty(panel, "getBoundingClientRect", {
            configurable: true,
            value: () => null,
        });

        const expandButton = panel.querySelector('button[title="Expand panel"]') as HTMLButtonElement;
        const resizeCorner = panel.querySelector('[title="Resize panel"]') as HTMLElement;

        act(() => {
            mouseDown(expandButton, 0, 0);
            mouseDown(resizeCorner, 200, 200);
            dispatchMouseMove(220, 220);
            dispatchMouseUp(220, 220);
        });

        expect(onExpand).toHaveBeenCalledTimes(0);
        expect(onResize).toHaveBeenCalledTimes(0);
    });

    test("collapsed keydown safely no-ops when onExpand is absent", () => {
        act(() => {
            root.render(
                <ResizablePanel
                    id="panel-no-expand"
                    x={20}
                    y={30}
                    width={280}
                    height={220}
                    isCollapsed
                >
                    hidden
                </ResizablePanel>,
            );
        });

        const launcher = container.querySelector("div") as HTMLDivElement;
        act(() => {
            launcher.dispatchEvent(new KeyboardEvent("keydown", { bubbles: true, key: "Enter" }));
        });

        expect(launcher.getAttribute("role")).toBeNull();
    });

    test("handles header mousedown with null panel rect without entering drag", async () => {
        const onMove = mock(() => { });

        act(() => {
            root.render(
                <ResizablePanel
                    id="panel-null-rect"
                    title="Null Rect"
                    x={60}
                    y={60}
                    width={300}
                    height={220}
                    onMove={onMove}
                >
                    test
                </ResizablePanel>,
            );
        });

        const panel = container.querySelector('[data-panel-id="panel-null-rect"]') as HTMLElement;
        Object.defineProperty(panel, "getBoundingClientRect", {
            configurable: true,
            value: () => null,
        });

        const header = Array.from(panel.children).find(
            (node) => (node as HTMLElement).style.cursor === "grab",
        ) as HTMLElement;

        act(() => {
            mouseDown(header, 70, 70);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(120, 120);
            dispatchMouseUp(120, 120);
        });

        expect(onMove).toHaveBeenCalledTimes(0);
    });

    test("supports multiple snap sides and lock transitions", async () => {
        const onAnchorCommit = mock(() => { });

        act(() => {
            root.render(
                <ResizablePanel
                    id="panel-snap"
                    title="Snap"
                    x={100}
                    y={100}
                    width={300}
                    height={220}
                    onMove={() => { }}
                    onAnchorCommit={onAnchorCommit}
                >
                    snap
                </ResizablePanel>,
            );
        });

        const panel = container.querySelector('[data-panel-id="panel-snap"]') as HTMLElement;
        const header = Array.from(panel.children).find(
            (node) => (node as HTMLElement).style.cursor === "grab",
        ) as HTMLElement;

        const setPanelRect = (left: number, top: number) => {
            Object.defineProperty(panel, "getBoundingClientRect", {
                configurable: true,
                value: () => ({
                    x: left,
                    y: top,
                    left,
                    top,
                    width: 300,
                    height: 220,
                    right: left + 300,
                    bottom: top + 220,
                    toJSON: () => ({}),
                }),
            });
        };

        const makeTarget = (id: string, left: number, top: number, width: number, height: number) => {
            const target = document.createElement("div");
            target.dataset.resizablePanel = "true";
            target.dataset.panelId = id;
            Object.defineProperty(target, "getBoundingClientRect", {
                configurable: true,
                value: () => ({
                    x: left,
                    y: top,
                    left,
                    top,
                    width,
                    height,
                    right: left + width,
                    bottom: top + height,
                    toJSON: () => ({}),
                }),
            });
            document.body.appendChild(target);
            return target;
        };

        const noIdCandidate = document.createElement("div");
        noIdCandidate.dataset.resizablePanel = "true";
        document.body.appendChild(noIdCandidate);

        // Horizontal right-side snap (sourceSide left)
        const rightNeighbor = makeTarget("neighbor-right", 404, 100, 220, 220);
        setPanelRect(100, 100);
        act(() => {
            mouseDown(header, 120, 120);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(140, 120);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(260, 120);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseUp(260, 120);
        });

        // Vertical top snap (sourceSide bottom)
        const topNeighbor = makeTarget("neighbor-top", 100, 40, 300, 40);
        setPanelRect(100, 100);
        act(() => {
            mouseDown(header, 120, 120);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(120, 58);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseUp(120, 58);
        });

        // Vertical bottom snap (sourceSide top)
        const bottomNeighbor = makeTarget("neighbor-bottom", 100, 336, 300, 100);
        setPanelRect(100, 100);
        act(() => {
            mouseDown(header, 120, 120);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(120, 236);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseUp(120, 236);
        });

        expect(onAnchorCommit).toHaveBeenCalled();

        rightNeighbor.remove();
        topNeighbor.remove();
        bottomNeighbor.remove();
        noIdCandidate.remove();
    });

    test("commits right-side snap when target is on the left", async () => {
        const onAnchorCommit = mock(() => { });

        act(() => {
            root.render(
                <ResizablePanel
                    id="panel-right-snap"
                    title="Right Snap"
                    x={100}
                    y={100}
                    width={300}
                    height={220}
                    onMove={() => { }}
                    onAnchorCommit={onAnchorCommit}
                >
                    right snap
                </ResizablePanel>,
            );
        });

        const panel = container.querySelector('[data-panel-id="panel-right-snap"]') as HTMLElement;
        const header = Array.from(panel.children).find(
            (node) => (node as HTMLElement).style.cursor === "grab",
        ) as HTMLElement;

        Object.defineProperty(panel, "getBoundingClientRect", {
            configurable: true,
            value: () => ({
                x: 100,
                y: 100,
                left: 100,
                top: 100,
                width: 300,
                height: 220,
                right: 400,
                bottom: 320,
                toJSON: () => ({}),
            }),
        });

        const leftTarget = document.createElement("div");
        leftTarget.dataset.resizablePanel = "true";
        leftTarget.dataset.panelId = "target-left";
        Object.defineProperty(leftTarget, "getBoundingClientRect", {
            configurable: true,
            value: () => ({
                x: 40,
                y: 100,
                left: 40,
                top: 100,
                width: 50,
                height: 220,
                right: 90,
                bottom: 320,
                toJSON: () => ({}),
            }),
        });
        document.body.appendChild(leftTarget);

        act(() => {
            mouseDown(header, 120, 120);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(114, 120);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseUp(114, 120);
        });

        expect(onAnchorCommit).toHaveBeenCalledWith("target-left", "right");
        leftTarget.remove();
    });

    test("commits bottom and top snap sides with vertical targets", async () => {
        const onAnchorCommit = mock(() => { });

        act(() => {
            root.render(
                <ResizablePanel
                    id="panel-vertical-snap"
                    title="Vertical Snap"
                    x={100}
                    y={100}
                    width={300}
                    height={220}
                    onMove={() => { }}
                    onAnchorCommit={onAnchorCommit}
                >
                    vertical
                </ResizablePanel>,
            );
        });

        const panel = container.querySelector('[data-panel-id="panel-vertical-snap"]') as HTMLElement;
        const header = Array.from(panel.children).find(
            (node) => (node as HTMLElement).style.cursor === "grab",
        ) as HTMLElement;

        Object.defineProperty(panel, "getBoundingClientRect", {
            configurable: true,
            value: () => ({
                x: 100,
                y: 100,
                left: 100,
                top: 100,
                width: 300,
                height: 220,
                right: 400,
                bottom: 320,
                toJSON: () => ({}),
            }),
        });

        const topTarget = document.createElement("div");
        topTarget.dataset.resizablePanel = "true";
        topTarget.dataset.panelId = "target-top";
        Object.defineProperty(topTarget, "getBoundingClientRect", {
            configurable: true,
            value: () => ({
                x: 100,
                y: 20,
                left: 100,
                top: 20,
                width: 300,
                height: 70,
                right: 400,
                bottom: 90,
                toJSON: () => ({}),
            }),
        });

        const bottomTarget = document.createElement("div");
        bottomTarget.dataset.resizablePanel = "true";
        bottomTarget.dataset.panelId = "target-bottom";
        Object.defineProperty(bottomTarget, "getBoundingClientRect", {
            configurable: true,
            value: () => ({
                x: 100,
                y: 324,
                left: 100,
                top: 324,
                width: 300,
                height: 80,
                right: 400,
                bottom: 404,
                toJSON: () => ({}),
            }),
        });

        document.body.appendChild(topTarget);
        document.body.appendChild(bottomTarget);

        act(() => {
            mouseDown(header, 120, 120);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(120, 114);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseUp(120, 114);
        });

        act(() => {
            mouseDown(header, 120, 120);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(120, 124);
            dispatchMouseMove(120, 124);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseUp(120, 124);
        });

        expect(onAnchorCommit).toHaveBeenCalledWith("target-top", "bottom");
        expect(onAnchorCommit).toHaveBeenCalledWith("target-bottom", "top");

        topTarget.remove();
        bottomTarget.remove();
    });

    test("reuses sticky candidate across repeated moves", async () => {
        const onAnchorCommit = mock(() => { });

        act(() => {
            root.render(
                <ResizablePanel
                    id="panel-sticky"
                    title="Sticky"
                    x={100}
                    y={100}
                    width={300}
                    height={220}
                    onMove={() => { }}
                    onAnchorCommit={onAnchorCommit}
                >
                    sticky
                </ResizablePanel>,
            );
        });

        const panel = container.querySelector('[data-panel-id="panel-sticky"]') as HTMLElement;
        const header = Array.from(panel.children).find(
            (node) => (node as HTMLElement).style.cursor === "grab",
        ) as HTMLElement;

        Object.defineProperty(panel, "getBoundingClientRect", {
            configurable: true,
            value: () => ({
                x: 100,
                y: 100,
                left: 100,
                top: 100,
                width: 300,
                height: 220,
                right: 400,
                bottom: 320,
                toJSON: () => ({}),
            }),
        });

        const target = document.createElement("div");
        target.dataset.resizablePanel = "true";
        target.dataset.panelId = "target-sticky";
        Object.defineProperty(target, "getBoundingClientRect", {
            configurable: true,
            value: () => ({
                x: 40,
                y: 100,
                left: 40,
                top: 100,
                width: 50,
                height: 220,
                right: 90,
                bottom: 320,
                toJSON: () => ({}),
            }),
        });
        document.body.appendChild(target);

        act(() => {
            mouseDown(header, 120, 120);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(114, 120);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(114, 120);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseUp(114, 120);
        });

        expect(onAnchorCommit).toHaveBeenCalledWith("target-sticky", "right");
        target.remove();
    });

    test("handles locked-candidate target removal and overlap loss guards", async () => {
        const onAnchorCommit = mock(() => { });

        act(() => {
            root.render(
                <ResizablePanel
                    id="panel-lock-guards"
                    title="Lock Guards"
                    x={100}
                    y={100}
                    width={300}
                    height={220}
                    onMove={() => { }}
                    onAnchorCommit={onAnchorCommit}
                >
                    lock guards
                </ResizablePanel>,
            );
        });

        const panel = container.querySelector('[data-panel-id="panel-lock-guards"]') as HTMLElement;
        const header = Array.from(panel.children).find(
            (node) => (node as HTMLElement).style.cursor === "grab",
        ) as HTMLElement;

        Object.defineProperty(panel, "getBoundingClientRect", {
            configurable: true,
            value: () => ({
                x: 100,
                y: 100,
                left: 100,
                top: 100,
                width: 300,
                height: 220,
                right: 400,
                bottom: 320,
                toJSON: () => ({}),
            }),
        });

        const target = document.createElement("div");
        target.dataset.resizablePanel = "true";
        target.dataset.panelId = "target-guards";
        Object.defineProperty(target, "getBoundingClientRect", {
            configurable: true,
            value: () => ({
                x: 40,
                y: 100,
                left: 40,
                top: 100,
                width: 50,
                height: 220,
                right: 90,
                bottom: 320,
                toJSON: () => ({}),
            }),
        });
        document.body.appendChild(target);

        act(() => {
            mouseDown(header, 120, 120);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(114, 120);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseMove(114, 420);
        });
        await Promise.resolve();

        target.remove();

        act(() => {
            dispatchMouseMove(114, 120);
        });
        await Promise.resolve();
        act(() => {
            dispatchMouseUp(114, 120);
        });

        expect(onAnchorCommit).toHaveBeenCalledTimes(0);
    });

    test("covers locked bottom resolution and missing locked target branch", async () => {
        const onAnchorCommit = mock(() => { });

        act(() => {
            root.render(
                <ResizablePanel
                    id="panel-bottom-lock"
                    title="Bottom Lock"
                    x={100}
                    y={100}
                    width={300}
                    height={220}
                    onMove={() => { }}
                    onAnchorCommit={onAnchorCommit}
                >
                    bottom lock
                </ResizablePanel>,
            );
        });

        const panel = container.querySelector('[data-panel-id="panel-bottom-lock"]') as HTMLElement;
        const header = Array.from(panel.children).find(
            (node) => (node as HTMLElement).style.cursor === "grab",
        ) as HTMLElement;

        Object.defineProperty(panel, "getBoundingClientRect", {
            configurable: true,
            value: () => ({
                x: 100,
                y: 100,
                left: 100,
                top: 100,
                width: 300,
                height: 220,
                right: 400,
                bottom: 320,
                toJSON: () => ({}),
            }),
        });

        const topTarget = document.createElement("div");
        topTarget.dataset.resizablePanel = "true";
        topTarget.dataset.panelId = "target-bottom-lock";
        Object.defineProperty(topTarget, "getBoundingClientRect", {
            configurable: true,
            value: () => ({
                x: 100,
                y: 20,
                left: 100,
                top: 20,
                width: 300,
                height: 70,
                right: 400,
                bottom: 90,
                toJSON: () => ({}),
            }),
        });
        document.body.appendChild(topTarget);

        act(() => {
            mouseDown(header, 120, 120);
        });
        await Promise.resolve();

        // First move: set bottom snap candidate and lock.
        act(() => {
            dispatchMouseMove(120, 114);
        });
        await Promise.resolve();

        // Second move: re-evaluate while locked (bottom branch).
        act(() => {
            dispatchMouseMove(120, 114);
        });
        await Promise.resolve();

        // Remove target to trigger missing-target guard in locked resolution.
        topTarget.remove();

        act(() => {
            dispatchMouseMove(120, 114);
        });
        await Promise.resolve();

        act(() => {
            dispatchMouseUp(120, 114);
        });

        expect(onAnchorCommit).toHaveBeenCalledTimes(0);
    });
});
