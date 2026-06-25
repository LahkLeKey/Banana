import type { Meta, StoryObj } from '@storybook/react';
import { PanelRelationshipsShowcase } from './interactions/InteractionShowcase';

const meta: Meta = {
    title: 'Panels/Interactions/Panel relationships',
    parameters: {
        layout: 'fullscreen',
        controls: { expanded: true },
    },
};

export default meta;

type Story = StoryObj;

export const ScopedInteractions: Story = {
    name: 'Scoped interactions',
    render: () => <PanelRelationshipsShowcase isolateExternalScope />,
};

export const SharedInteractions: Story = {
    name: 'Shared interactions',
    render: () => <PanelRelationshipsShowcase isolateExternalScope={false} />,
};