import type {NotebookDocument} from './types';

function asSourceLines(source: string[]|string|undefined): string[] {
  if (Array.isArray(source)) {
    return source;
  }
  if (typeof source === 'string') {
    return [source];
  }
  return [];
}

function normalizeHeaderLine(rawHeader: string): string {
  return rawHeader.replace(/(?:\\r\\n|\\n|\\r)+$/g, '').trim();
}

export function buildCellLookup(notebook: NotebookDocument|null):
    Map<string, string> {
  const lookup = new Map<string, string>();
  const cells = notebook?.cells ?? [];

  for (const cell of cells) {
    const sourceLines = asSourceLines(cell.source);
    if (cell.cell_type !== 'code' || sourceLines.length === 0) {
      continue;
    }

    const header = normalizeHeaderLine(sourceLines[0] ?? '');
    if (!header.startsWith('// source:')) {
      continue;
    }

    const filePath = header.replace(/^\/\/\s*source:\s*/, '').trim();
    const rawContent = sourceLines.slice(1).join('');
    const content = rawContent.replace(/\\n/g, '\n')
                        .replace(/\\t/g, '\t')
                        .replace(/\\r/g, '')
                        .replace(/\\\\/g, '\\');
    lookup.set(filePath, content);
  }

  return lookup;
}
