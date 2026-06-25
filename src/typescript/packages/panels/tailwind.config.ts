import type {Config} from 'tailwindcss';

import panelTailwindPreset from './src/tailwind/preset';

const config: Config = {
  content: [
    './src/**/*.{ts,tsx}',
    './.storybook/**/*.{ts,tsx,js,jsx}',
  ],
  presets: [panelTailwindPreset as Config],
  theme: {
    extend: {},
  },
};

export default config;
