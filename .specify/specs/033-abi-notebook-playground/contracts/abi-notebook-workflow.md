# ABI Notebook Workflow Contract

## Objective

Provide a deterministic workflow that exports native C and header files into a Jupyter notebook artifact for exploratory prototyping and analysis.

## Inputs

- Source root: `src/native`
- File patterns: `*.c`, `*.h`
- Exclusions: `out/`, `.git/`, generated build directories

## Outputs

- Notebook artifact path: `artifacts/notebooks/native-c-catalog.ipynb`
- Notebook format: JSON notebook document with markdown index cell plus code cells per source file

## Behavioral Guarantees

1. File ordering is deterministic (lexicographic path order).
2. Notebook generation is idempotent for identical source trees.
3. Each code cell includes a heading comment with workspace-relative source path.
4. Large files are truncated by configurable maximum line count per file to keep notebooks interactive.

## Failure Handling

- If source root does not exist, generator exits non-zero with actionable message.
- If no matching files are found, generator exits non-zero.
- If output directory does not exist, orchestrator creates it before generation.

## Validation Commands

- `bash scripts/scaffold-abi-notebook-workflow.sh`
- `python scripts/export-native-c-to-notebook.py --source-root src/native --output artifacts/notebooks/native-c-catalog.ipynb`
