# Notebooks Workspace

This folder is the canonical workspace location for generated notebook artifacts.

Primary generated files:

- `notebooks/native-c-catalog.ipynb`
- `notebooks/catalog-index.json`

Generate/update with:

```bash
bash scripts/scaffold-abi-notebook-workflow.sh
```

Deploy behavior:

- The scaffold script also publishes copies into `src/typescript/react/public/notebooks/` for Vercel static hosting.
- Additional copies are written under `artifacts/notebooks/` for evidence retention.
