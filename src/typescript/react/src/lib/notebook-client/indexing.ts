import type {NotebookDocument} from './types';

export function buildCellLookup(notebook: NotebookDocument|null):
    Map<string, string> {
  const lookup = new Map<string, string>();
  const cells = notebook?.cells ?? [];

  for (const cell of cells) {
    if (cell.cell_type !== 'code' || !Array.isArray(cell.source) ||
        cell.source.length === 0) {
      continue;
    }

    const header = cell.source[0] ?? '';
    if (!header.startsWith('// source: ')) {
      continue;
    }

    const filePath = header.replace('// source: ', '').trim();
    const content = cell.source.slice(1).join('');
    lookup.set(filePath, content);
  }

  return lookup;
}
