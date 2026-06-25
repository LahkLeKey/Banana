import type { Meta, StoryObj } from '@storybook/react';
import { DockingLabelsShowcase } from './interactions/InteractionShowcase';

const meta: Meta = {
    title: 'Panels/Interactions/Docking labels',
    parameters: {
        layout: 'fullscreen',
        controls: { expanded: true },
    },
};

export default meta;

type Story = StoryObj;

export const Default: Story = {
    render: () => <DockingLabelsShowcase />,
};