import type { Meta, StoryObj } from '@storybook/react';
import { DragSnapShowcase } from './interactions/InteractionShowcase';

const meta: Meta = {
    title: 'Panels/Interactions/Drag and snap',
    parameters: {
        layout: 'fullscreen',
        controls: { expanded: true },
    },
};

export default meta;

type Story = StoryObj;

export const Default: Story = {
    render: () => <DragSnapShowcase />,
};

export const IsolatedScopeGuard: Story = {
    name: 'Isolated scope guard',
    render: () => <DragSnapShowcase isolateThirdPanel />,
};