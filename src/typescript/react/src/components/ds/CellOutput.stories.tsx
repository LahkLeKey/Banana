import type { Meta, StoryObj } from "@storybook/react-vite";
import { CellOutput } from "./CellOutput";

const meta = {
    title: "DS/CellOutput",
    component: CellOutput,
    parameters: { layout: "padded" },
    tags: ["autodocs"],
} satisfies Meta<typeof CellOutput>;

export default meta;
type Story = StoryObj<typeof meta>;

export const StdoutOnly: Story = {
    args: {
        execCount: 4,
        output: "pyodide_exec_ok\nmodel_accuracy=0.94",
    },
};

export const ErrorOnly: Story = {
    args: {
        execCount: 5,
        error: "Traceback (most recent call last):\n  File \"<exec>\", line 1\nSyntaxError: invalid syntax",
    },
};

export const TableAndContract: Story = {
    args: {
        execCount: 12,
        output: "rendered 2 artifacts",
        richOutput: [
            {
                kind: "table",
                table: {
                    columns: ["lane", "accuracy", "f1"],
                    rows: [
                        ["banana", 0.94, 0.93],
                        ["not-banana", 0.91, 0.9],
                    ],
                },
            },
            {
                kind: "contract",
                family: "plotly",
                status: "active",
                title: "Plotly Renderer Contract",
                summary: "Plotly artifacts render as interactive charts in the output panel.",
                detail: "No fallback path taken for this output.",
                marker: "BANANA_PLOTLY::",
            },
        ],
    },
};
