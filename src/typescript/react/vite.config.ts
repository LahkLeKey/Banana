import {sentryVitePlugin} from "@sentry/vite-plugin";
import react from "@vitejs/plugin-react";
import {defineConfig, loadEnv} from "vite";

// Compile-time injection (spec 009 contract): VITE_BANANA_API_BASE_URL.
// Spec 131: throw on missing required vars in production.
// Spec 133: vendor manualChunks for smaller initial JS transfer.
// Spec 138: Sentry source-map upload when SENTRY_AUTH_TOKEN is present.
export default defineConfig(({mode}) => {
    const env = loadEnv(mode, process.cwd(), "VITE_");

    if (mode === "production" && !env.VITE_BANANA_API_BASE_URL)
    {
        throw new Error("[Banana] VITE_BANANA_API_BASE_URL is required for production builds. " +
                        "Set it in your Vercel environment variables or .env.production.local.");
    }

    return {
    plugins: [
      react(),
      ...(process.env.SENTRY_AUTH_TOKEN
        ? [
            sentryVitePlugin({
              org: "banana",
              project: "banana-react",
            }),
          ]
        : []),
    ],
    server: { host: "0.0.0.0", port: 5173 },
    build: {
      sourcemap: process.env.SENTRY_AUTH_TOKEN ? "hidden" : false,
      rollupOptions: {
        output: {
          manualChunks: {
            vendor: ["react", "react-dom"],
            router: ["react-router-dom"],
            radix: ["@radix-ui/react-slot"],
            query: ["@tanstack/react-query"],
          },
        },
      },
    },
  };
});
