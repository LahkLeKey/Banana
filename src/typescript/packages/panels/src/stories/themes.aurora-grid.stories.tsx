import type { Meta, StoryObj } from '@storybook/react';
import { ThemeCategoryShowcase, type ThemeCategoryStoryArgs } from './themes/ThemeCategoryShowcase';

const meta: Meta<ThemeCategoryStoryArgs> = {
    title: 'Panels/Themes/Aurora Grid',
    parameters: {
        layout: 'fullscreen',
        controls: { expanded: true },
    },
    args: {
        mode: 'light',
        accentOverride: '#2563eb',
        showLegend: false,
        showDockedComponents: false,
        showCommandWindow: true,
        showInsightsWindow: true,
        showAuditWindow: true,
    },
    argTypes: {
        mode: {
            options: ['light', 'dark'],
            control: { type: 'inline-radio' },
        },
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

export const Light: Story = {
    name: 'Light',
    args: {
        mode: 'light',
        accentOverride: '#2563eb',
    },
    render: (args) => <ThemeCategoryShowcase category="aurora-grid" args={args} />,
};

export const Dark: Story = {
    name: 'Dark',
    args: {
        mode: 'dark',
        accentOverride: '#60a5fa',
    },
    render: (args) => <ThemeCategoryShowcase category="aurora-grid" args={args} />,
};
