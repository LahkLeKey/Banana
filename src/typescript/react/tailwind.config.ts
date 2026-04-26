/** @type {import('tailwindcss').Config} */
import type {Config} from 'tailwindcss';
const config: Config = {
  content:
      ['./index.html', './src/**/*.{ts,tsx}', '../shared/ui/src/**/*.{ts,tsx}'],
  theme: {extend: {}},
  plugins: [],
};
export default config;
