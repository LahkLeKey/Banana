import type { Meta, StoryObj } from "@storybook/react-vite";
import { fn } from "storybook/test";
import { NotebookToolbar } from "./NotebookToolbar";

const meta = {
    title: "DS/NotebookToolbar",
    component: NotebookToolbar,
    parameters: { layout: "padded" },
    tags: ["autodocs"],
    args: {
        notebookLabel: "07 Visualization Workbench",
        cellCount: 9,
        executedCount: 6,
        runtimeState: "ready",
        runtimeMessage: "Pyodide runtime loaded",
        showLibrary: true,
        onRunAll: fn(),
        onSave: fn(),
        onExport: fn(),
        onReset: fn(),
        onToggleCatalog: fn(),
        onWarmRuntime: fn(),
    },
} satisfies Meta<typeof NotebookToolbar>;

export default meta;
type Story = StoryObj<typeof meta>;

export const RuntimeReady: Story = {};

export const RuntimeIdle: Story = {
    args: {
        runtimeState: "idle",
        runtimeMessage: "Click to warm runtime",
        showLibrary: false,
    },
};

export const RuntimeLoading: Story = {
    args: {
        runtimeState: "loading",
        runtimeMessage: "Loading Pyodide packages...",
    },
};

export const RuntimeError: Story = {
    args: {
        runtimeState: "error",
        runtimeMessage: "Failed to initialize runtime",
    },
};
