import bananaUiPreset from '@banana/ui/tailwind/preset';
import type {Config} from 'tailwindcss';

export default {
  presets: [bananaUiPreset],
  content:
      [
        './index.html', './src/**/*.{ts,tsx}',
        './node_modules/@banana/ui/src/**/*.{ts,tsx}'
      ],
  theme: {
    extend: {
      fontFamily: {
        display: ['Space Grotesk', 'Segoe UI', 'sans-serif'],
        body: ['Manrope', 'Segoe UI', 'sans-serif']
      },
      boxShadow: {panel: '0 20px 45px -25px rgba(15, 23, 42, 0.4)'}
    }
  },
  plugins: []
} satisfies Config;
