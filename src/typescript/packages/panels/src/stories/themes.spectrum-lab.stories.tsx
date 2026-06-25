import type { Meta, StoryObj } from '@storybook/react';
import { ThemeCategoryShowcase, type ThemeCategoryStoryArgs } from './themes/ThemeCategoryShowcase';

const meta: Meta<ThemeCategoryStoryArgs> = {
    title: 'Panels/Themes/Spectrum Lab',
    parameters: {
        layout: 'fullscreen',
        controls: { expanded: true },
    },
    args: {
        mode: 'dark',
        accentOverride: '#22d3ee',
        showLegend: false,
        showDockedComponents: false,
        showCommandWindow: true,
        showInsightsWindow: true,
        showAuditWindow: true,
    },
    argTypes: {
        mode: { control: false },
        themeName: { control: false },
        accentOverride: { control: 'text' },
        showLegend: { control: 'boolean' },
        showDockedComponents: { control: 'boolean' },
        showCommandWindow: { control: 'boolean' },
        showInsightsWindow: { control: 'boolean' },
        showAuditWindow: { control: 'boolean' },
    },
};

export default meta;

type Story = StoryObj<ThemeCategoryStoryArgs>;

export const Abyss: Story = {
    name: 'Abyss',
    args: {
        themeName: 'deep-space',
        accentOverride: '#22d3ee',
    },
    render: (args) => <ThemeCategoryShowcase category="spectrum-lab" args={args} />,
};

export const Ember: Story = {
    name: 'Ember',
    args: {
        themeName: 'sunset-grid',
        accentOverride: '#fb7185',
    },
    render: (args) => <ThemeCategoryShowcase category="spectrum-lab" args={args} />,
};

export const Verdant: Story = {
    name: 'Verdant',
    args: {
        themeName: 'mint-circuit',
        accentOverride: '#2dd4bf',
    },
    render: (args) => <ThemeCategoryShowcase category="spectrum-lab" args={args} />,
};
