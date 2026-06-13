import {resolveApiBaseUrl} from '../api';

import type {NotebookDocument, NotebookManifest} from './types';

async function readJson<T>(url: string): Promise<T> {
  const response = await fetch(url);
  if (!response.ok) {
    throw new Error(`failed to fetch ${url}`);
  }
  return response.json() as Promise<T>;
}

function joinUrl(baseUrl: string, path: string): string {
  return `${baseUrl.replace(/\/$/, '')}${path}`;
}

export function resolveNotebookApiBaseUrl(): string {
  return resolveApiBaseUrl().trim();
}

function manifestCandidates(apiBaseUrl: string): string[] {
  return [joinUrl(apiBaseUrl, '/api/notebooks/catalog-index.json')];
}

function notebookCandidates(apiBaseUrl: string): string[] {
  return [joinUrl(apiBaseUrl, '/api/notebooks/native-c-catalog.ipynb')];
}

async function fetchFirstJson<T>(candidates: string[]):
    Promise<{payload: T; source: string}> {
  let lastError: unknown = null;
  for (const candidate of candidates) {
    try {
      const payload = await readJson<T>(candidate);
      return {payload, source: candidate};
    } catch (error) {
      lastError = error;
    }
  }

  throw lastError ?? new Error('unable to load json payload');
}

export async function fetchNotebookManifestFromAssets():
    Promise<NotebookManifest> {
  return readJson<NotebookManifest>('/notebooks/catalog-index.json');
}

export async function fetchNotebookDocumentFromAssets():
    Promise<NotebookDocument> {
  return readJson<NotebookDocument>('/notebooks/native-c-catalog.ipynb');
}

export async function fetchNotebookManifest(): Promise<NotebookManifest> {
  const {payload} = await fetchNotebookManifestWithSource();
  return payload;
}

export async function fetchNotebookDocument(): Promise<NotebookDocument> {
  const {payload} = await fetchNotebookDocumentWithSource();
  return payload;
}

export async function fetchNotebookManifestWithSource(
    apiBaseUrl = resolveNotebookApiBaseUrl()):
    Promise<{payload: NotebookManifest; source: string}> {
  if (apiBaseUrl.length === 0) {
    throw new Error(
        'Notebook API base URL is unavailable. Configure VITE_BANANA_API_BASE_URL.');
  }
  return fetchFirstJson<NotebookManifest>(manifestCandidates(apiBaseUrl));
}

export async function fetchNotebookDocumentWithSource(
    apiBaseUrl = resolveNotebookApiBaseUrl()):
    Promise<{payload: NotebookDocument; source: string}> {
  if (apiBaseUrl.length === 0) {
    throw new Error(
        'Notebook API base URL is unavailable. Configure VITE_BANANA_API_BASE_URL.');
  }
  return fetchFirstJson<NotebookDocument>(notebookCandidates(apiBaseUrl));
}
