import type { Meta, StoryObj } from '@storybook/react';
import { DialogOverlayShowcase } from './interactions/InteractionShowcase';

const meta: Meta = {
    title: 'Panels/Interactions/Dialog and overlay',
    parameters: {
        layout: 'fullscreen',
        controls: { expanded: true },
    },
};

export default meta;

type Story = StoryObj;

export const BackdropClosable: Story = {
    name: 'Backdrop closable',
    render: () => <DialogOverlayShowcase closeOnBackdropClick closeOnEscape />,
};

export const BackdropLocked: Story = {
    name: 'Backdrop locked',
    render: () => <DialogOverlayShowcase closeOnBackdropClick={false} closeOnEscape={false} />,
};