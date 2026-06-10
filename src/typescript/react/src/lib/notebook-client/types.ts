export type NotebookManifest = {
  generated_at_utc: string; source_root: string; notebook_path: string;
  file_count: number;
  max_lines_per_file: number;
  files: string[];
};

export type NotebookCell = {
  cell_type: 'markdown'|'code';
  source?: string[] | string;
};

export type NotebookDocument = {
  cells?: NotebookCell[];
};

export type NotebookClientLoadState = {
  manifest: NotebookManifest|null; notebook: NotebookDocument | null;
  manifestError: string | null;
  notebookError: string | null;
  manifestSource: string;
  notebookSource: string;
  loading: boolean;
};
