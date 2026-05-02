/// <reference types="vite/client" />
interface ImportMetaEnv {
  readonly VITE_BANANA_API_BASE_URL: string;
  readonly VITE_BANANA_CHAT_API_BASE_URL?: string;
}
interface ImportMeta {
  readonly env: ImportMetaEnv;
}

interface Window {
  banana?: { apiBaseUrl: string; platform: string; chatApiBaseUrl?: string };
}
