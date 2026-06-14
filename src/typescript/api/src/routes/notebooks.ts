// Notebooks route — serves a live-scanned catalog-index manifest of all
// native C/H sources so the file explorer always reflects the current tree.
import type {FastifyInstance} from 'fastify';
import {readdirSync, readFileSync, statSync} from 'node:fs';
import {join, relative} from 'node:path';

function discoverNativeSources(sourceRoot: string): string[] {
  const results: string[] = [];
  const walk = (dir: string) => {
    let entries: string[];
    try {
      entries = readdirSync(dir);
    } catch {
      return;
    }
    for (const entry of entries) {
      if (entry.startsWith('.')) continue;
      const full = join(dir, entry);
      let stat;
      try {
        stat = statSync(full);
      } catch {
        continue;
      }
      if (stat.isDirectory()) {
        walk(full);
      } else if (/\.[ch]$/.test(entry)) {
        results.push(full);
      }
    }
  };
  walk(sourceRoot);
  results.sort();
  return results;
}

type NotebookManifestPayload = {
  generated_at_utc: string; source_root: string; notebook_path: string;
  file_count: number;
  max_lines_per_file: number;
  files: string[];
};

function loadBundledNotebookManifest(workspaceRoot: string):
    NotebookManifestPayload|null {
  try {
    const raw = readFileSync(
        join(workspaceRoot, 'notebooks', 'catalog-index.json'), 'utf8');
    const parsed = JSON.parse(raw) as Partial<NotebookManifestPayload>;
    if (!Array.isArray(parsed.files)) {
      return null;
    }

    return {
      generated_at_utc: new Date().toISOString(),
      source_root: typeof parsed.source_root === 'string' ? parsed.source_root :
                                                            'src/native',
      notebook_path: typeof parsed.notebook_path === 'string' ?
          parsed.notebook_path :
          'notebooks/native-c-catalog.ipynb',
      file_count: parsed.files.length,
      max_lines_per_file: typeof parsed.max_lines_per_file === 'number' ?
          parsed.max_lines_per_file :
          240,
      files: parsed.files,
    };
  } catch {
    return null;
  }
}

export async function registerNotebooksRoutes(app: FastifyInstance):
    Promise<void> {
  // GET /api/notebooks/catalog-index.json — live-scanned native source
  // manifest. Workspace root is resolved relative to this file's location at
  // runtime
  // (/workspace/src/typescript/api/src/routes/notebooks.ts → /workspace).
  app.get('/api/notebooks/catalog-index.json', async (_request, reply) => {
    const workspaceRoot =
        join(import.meta.dirname, '..', '..', '..', '..', '..');
    const sourceRoot = join(workspaceRoot, 'src', 'native');
    const files = discoverNativeSources(sourceRoot);

    if (files.length === 0) {
      const bundledManifest = loadBundledNotebookManifest(workspaceRoot);
      if (bundledManifest) {
        return reply.send(bundledManifest);
      }
    }

    const relativeFiles = files.map(
        (f) => relative(workspaceRoot, f).replace(/\\/g, '/'),
    );
    return reply.send({
      generated_at_utc: new Date().toISOString(),
      source_root: 'src/native',
      notebook_path: 'notebooks/native-c-catalog.ipynb',
      file_count: relativeFiles.length,
      max_lines_per_file: 240,
      files: relativeFiles,
    });
  });

  app.get('/api/notebooks/native-c-catalog.ipynb', async (_request, reply) => {
    const workspaceRoot =
        join(import.meta.dirname, '..', '..', '..', '..', '..');
    const payloadPath =
        join(workspaceRoot, 'notebooks', 'native-c-catalog.ipynb');

    try {
      const payload = readFileSync(payloadPath, 'utf8');
      reply.type('application/json; charset=utf-8');
      return reply.send(payload);
    } catch {
      return reply.status(404).send({
        message:
            'Notebook payload is unavailable from API. Check API runtime and notebook endpoint.',
      });
    }
  });
}
