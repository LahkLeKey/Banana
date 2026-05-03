import type { Meta, StoryObj } from "@storybook/react-vite";
import { fn } from "storybook/test";
import { NotebookCatalog } from "./NotebookCatalog";

const library = [
    {
        path: "public/notebooks/01-platform-setup.ipynb",
        label: "01 Platform Setup",
        description: "Bootstrap runtime and verify contracts",
    },
    {
        path: "public/notebooks/02-banana-classifier-workbench.ipynb",
        label: "02 Banana Classifier Workbench",
        description: "Train and compare classifier variants",
    },
    {
        path: "public/notebooks/07-visualization-workbench.ipynb",
        label: "07 Visualization Workbench",
        description: "Plotly and table render contracts",
    },
];

const meta = {
    title: "DS/NotebookCatalog",
    component: NotebookCatalog,
    parameters: { layout: "padded" },
    tags: ["autodocs"],
    args: {
        activeNotebookLabel: "07 Visualization Workbench",
        library,
        showAdvancedTools: false,
        onLoadNotebook: fn(),
        onClose: fn(),
        onAddPythonCell: fn(),
        onAddMarkdownCell: fn(),
        onToggleAdvancedTools: fn(),
    },
} satisfies Meta<typeof NotebookCatalog>;

export default meta;
type Story = StoryObj<typeof meta>;

export const Default: Story = {};

export const AdvancedToolsVisible: Story = {
    args: {
        showAdvancedTools: true,
    },
};
