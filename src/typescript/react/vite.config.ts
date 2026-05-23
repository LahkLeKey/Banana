import react from '@vitejs/plugin-react';
import {defineConfig, loadEnv} from 'vite';

// Compile-time injection (spec 009 contract): VITE_BANANA_API_BASE_URL.
// Spec 131: throw on missing required vars in production.
// Spec 133: vendor manualChunks for smaller initial JS transfer.
export default defineConfig(({mode}) => {
  const env = loadEnv(mode, process.cwd(), 'VITE_');
  const resolvedApiBaseUrl = env.VITE_BANANA_API_BASE_URL?.trim() ||
      (process.env.VERCEL_ENV === 'preview' ?
           'https://staging-api.banana.engineer' :
           'https://api.banana.engineer');

  const engineAssetVersion =
      process.env.VERCEL_GIT_COMMIT_SHA?.slice(0, 12) ?? 'local-dev';

  return {
    define: {
      'import.meta.env.VITE_BANANA_API_BASE_URL':
          JSON.stringify(resolvedApiBaseUrl),
      'import.meta.env.VITE_ENGINE_ASSET_VERSION':
          JSON.stringify(engineAssetVersion),
    },
    plugins: [react()],
    server: {host: '0.0.0.0', port: 5173},
    build: {
      sourcemap: false,
      rollupOptions: {
        output: {
          manualChunks: {
            vendor: ['react', 'react-dom'],
            router: ['react-router-dom'],
            radix: ['@radix-ui/react-slot'],
            query: ['@tanstack/react-query'],
          },
        },
      },
    },
  };
});
