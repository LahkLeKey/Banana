import type { Meta, StoryObj } from '@storybook/react';
import { ThemeCategoryShowcase, type ThemeCategoryStoryArgs } from './themes/ThemeCategoryShowcase';

const meta: Meta<ThemeCategoryStoryArgs> = {
    title: 'Panels/Themes/Tide Glass',
    parameters: {
        layout: 'fullscreen',
        controls: { expanded: true },
    },
    args: {
        mode: 'dark',
        accentOverride: '#60cdff',
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
        accentOverride: '#0078d4',
    },
    render: (args) => <ThemeCategoryShowcase category="tide-glass" args={args} />,
};

export const Dark: Story = {
    name: 'Dark',
    args: {
        mode: 'dark',
        accentOverride: '#60cdff',
    },
    render: (args) => <ThemeCategoryShowcase category="tide-glass" args={args} />,
};
