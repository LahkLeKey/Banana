import type { Meta, StoryObj } from '@storybook/react';
import React from 'react';

const meta: Meta = { title: 'Packages/@banana/panels-interactions' };
export default meta;

type Story = StoryObj;

export const PackageLoaded: Story = {
  render: () => React.createElement('div', null, 'Package story scaffold ready'),
};
