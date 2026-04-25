import react from '@vitejs/plugin-react';
import {defineConfig} from 'vite';

// Compile-time injection (spec 009 contract): VITE_BANANA_API_BASE_URL.
export default defineConfig({
  plugins: [react()],
  server: {host: '0.0.0.0', port: 5173},
});
