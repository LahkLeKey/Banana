// Notebooks route — serves a live-scanned catalog-index manifest of all
// native C/H sources so the file explorer always reflects the current tree.
import type {FastifyInstance} from 'fastify';
import {readdirSync, statSync} from 'node:fs';
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
}
