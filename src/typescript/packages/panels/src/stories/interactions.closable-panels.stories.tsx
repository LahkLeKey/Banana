import type { Meta, StoryObj } from '@storybook/react';
import { ClosablePanelsShowcase } from './interactions/InteractionShowcase';

const meta: Meta = {
    title: 'Panels/Interactions/Closable panels',
    parameters: {
        layout: 'fullscreen',
        controls: { expanded: true },
    },
};

export default meta;

type Story = StoryObj;

export const Default: Story = {
    render: () => <ClosablePanelsShowcase />,
};

export const StartsCollapsed: Story = {
    name: 'Starts collapsed',
    render: () => <ClosablePanelsShowcase startsCollapsed />,
};