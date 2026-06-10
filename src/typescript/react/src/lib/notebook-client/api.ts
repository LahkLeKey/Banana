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
  const candidates: string[] = ['/notebooks/catalog-index.json'];
  if (apiBaseUrl.length > 0) {
    candidates.push(joinUrl(apiBaseUrl, '/api/notebooks/catalog-index.json'));
    candidates.push(joinUrl(apiBaseUrl, '/notebooks/catalog-index.json'));
  }
  return candidates;
}

function notebookCandidates(apiBaseUrl: string): string[] {
  const candidates: string[] = ['/notebooks/native-c-catalog.ipynb'];
  if (apiBaseUrl.length > 0) {
    candidates.push(
        joinUrl(apiBaseUrl, '/api/notebooks/native-c-catalog.ipynb'));
    candidates.push(joinUrl(apiBaseUrl, '/notebooks/native-c-catalog.ipynb'));
  }
  return candidates;
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
  return fetchFirstJson<NotebookManifest>(manifestCandidates(apiBaseUrl));
}

export async function fetchNotebookDocumentWithSource(
    apiBaseUrl = resolveNotebookApiBaseUrl()):
    Promise<{payload: NotebookDocument; source: string}> {
  return fetchFirstJson<NotebookDocument>(notebookCandidates(apiBaseUrl));
}
