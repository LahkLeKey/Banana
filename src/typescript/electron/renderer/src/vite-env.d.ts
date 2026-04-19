/// <reference types="vite/client" />

type BananaQaConfig = {
  apiUrl: string; nativePath: string | null;
};

type BananaQaRequest = {
  purchases: number; multiplier: number; apiUrl: string;
  includeDatabase: boolean;
};

type BananaQaBridge = {
  getConfig: () => Promise<BananaQaConfig>;
  checkHealth: (payload: {apiUrl: string}) => Promise<unknown>;
  runNative: (payload: BananaQaRequest) => Promise<unknown>;
  runApi: (payload: BananaQaRequest) => Promise<unknown>;
  runCompare: (payload: BananaQaRequest) => Promise<unknown>;
};

declare global {
  interface Window {
    bananaQA: BananaQaBridge;
  }
}

export {};
