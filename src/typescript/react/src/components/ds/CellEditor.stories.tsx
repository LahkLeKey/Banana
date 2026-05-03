import type { Meta, StoryObj } from "@storybook/react-vite";
import { useState } from "react";
import { fn } from "storybook/test";
import type { NotebookCell } from "../../lib/dsTypes";
import { CellEditor } from "./CellEditor";

function CellEditorHarness({
    initialCell,
    cellIndex = 0,
}: {
    initialCell: NotebookCell;
    cellIndex?: number;
}) {
    const [cell, setCell] = useState<NotebookCell>(initialCell);
    return (
        <CellEditor
            cell={cell}
            cellIndex={cellIndex}
            onUpdateSource={(source) => setCell((prev) => ({ ...prev, source }))}
            onRun={fn()}
            onDelete={fn()}
            onTogglePreview={() =>
                setCell((prev) => ({
                    ...prev,
                    previewMode: !(prev.previewMode ?? true),
                }))
            }
        />
    );
}

const meta = {
    title: "DS/CellEditor",
    component: CellEditorHarness,
    parameters: { layout: "padded" },
    tags: ["autodocs"],
} satisfies Meta<typeof CellEditorHarness>;

export default meta;
type Story = StoryObj<typeof meta>;

export const PythonCell: Story = {
    args: {
        initialCell: {
            id: 1,
            kind: "python",
            source: "import json\nprint(json.dumps({'hello': 'banana'}))",
            execCount: 7,
            output: '{"hello": "banana"}',
        },
        cellIndex: 0,
    },
};

export const PythonRunning: Story = {
    args: {
        initialCell: {
            id: 2,
            kind: "python",
            source: "print('training...')",
            running: true,
            execCount: 8,
        },
        cellIndex: 1,
    },
};

export const MarkdownPreview: Story = {
    args: {
        initialCell: {
            id: 3,
            kind: "markdown",
            source: "## Notes\n- Track false positives\n- Export plots",
            previewMode: true,
        },
        cellIndex: 2,
    },
};
