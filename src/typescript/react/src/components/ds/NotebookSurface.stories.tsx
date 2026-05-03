import type { Meta, StoryObj } from "@storybook/react-vite";
import { NotebookSurface } from "./NotebookSurface";
import { surfaceFixtures } from "./storybook/dsStoryFixtures";

const meta = {
    title: "DS/Notebook Surfaces",
    component: NotebookSurface,
    parameters: {
        layout: "padded",
    },
    decorators: [
        (Story) => (
            <div className="min-h-screen bg-zinc-950 p-6 text-white">
                <div className="mx-auto max-w-5xl">
                    <Story />
                </div>
            </div>
        ),
    ],
} satisfies Meta<typeof NotebookSurface>;

export default meta;

type Story = StoryObj<typeof meta>;

export const Vega: Story = {
    args: { surface: surfaceFixtures.vega },
};

export const Altair: Story = {
    args: { surface: surfaceFixtures.altair },
};

export const Bokeh: Story = {
    args: { surface: surfaceFixtures.bokeh },
};

export const Widgets: Story = {
    args: { surface: surfaceFixtures.widgets },
};

export const Ipympl: Story = {
    args: { surface: surfaceFixtures.ipympl },
};

export const Bqplot: Story = {
    args: { surface: surfaceFixtures.bqplot },
};

export const Geospatial: Story = {
    args: { surface: surfaceFixtures.geospatial },
};

export const Network: Story = {
    args: { surface: surfaceFixtures.network },
};

export const ThreeD: Story = {
    args: { surface: surfaceFixtures["three-d"] },
};

export const Canvas: Story = {
    args: { surface: surfaceFixtures.canvas },
};

export const AllFamilies: Story = {
    args: { surface: surfaceFixtures.vega },
    render: () => (
        <div className="grid gap-4">
            {Object.values(surfaceFixtures).map((surface) => (
                <NotebookSurface key={surface.family} surface={surface} />
            ))}
        </div>
    ),
};