import './tailwind.css';

import type {Preview} from '@storybook/react';
import * as React from 'react';

// Some bundled panel modules are emitted with React.createElement references.
// Make React available globally in Storybook preview runtime.
(globalThis as {React?: typeof React}).React = React;

const preview: Preview = {
  parameters: {
    controls: {expanded: true},
    layout: 'fullscreen',
    backgrounds: {
      default: 'os-slate',
      values: [
        {name: 'os-slate', value: '#020617'},
        {name: 'graphite', value: '#111827'},
      ],
    },
  },
};

export default preview;
