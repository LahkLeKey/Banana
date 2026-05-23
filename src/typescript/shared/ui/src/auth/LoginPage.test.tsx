// @ts-nocheck -- bun:test types are not part of app tsconfig.

import { afterEach, beforeEach, describe, expect, test } from "bun:test";
import { GlobalRegistrator } from "@happy-dom/global-registrator";
import { act } from "react";
import { createRoot, type Root } from "react-dom/client";

import { LoginPage } from "./LoginPage";

(globalThis as unknown as { IS_REACT_ACT_ENVIRONMENT: boolean }).IS_REACT_ACT_ENVIRONMENT = true;

let container: HTMLDivElement;
let root: Root;

beforeEach(() => {
    if (!(globalThis as unknown as { document?: Document }).document) {
        GlobalRegistrator.register({ url: "http://localhost:5173/" });
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

describe("LoginPage", () => {
    test("renders the Steam login prompt", () => {
        act(() => {
            root.render(<LoginPage />);
        });

        expect(container.textContent ?? "").toContain("Continue with Steam");
        expect(container.textContent ?? "").toContain("Enter the Banana viewport.");
    });
});
