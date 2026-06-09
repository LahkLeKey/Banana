# Quickstart: ABI Notebook Playground Pivot

## 1) Generate notebook artifact from native C sources

```bash
bash scripts/scaffold-abi-notebook-workflow.sh
```

Expected output:

- `artifacts/notebooks/native-c-catalog.ipynb`

## 2) Validate native compile and focused smoke

```bash
cmake --build out/v3-native -- -m:1
ctest -C Debug --test-dir out/v3-native -R "banana_native_feedback_loop_factory_(smoke|dx12_scaffold_smoke|negotiate_smoke|comeback_smoke|flank_smoke)" --output-on-failure
```

## 3) Validate React route surface quickly

```bash
bun -e "import './src/typescript/react/src/lib/router.tsx'; console.log('router-ok');"
```

## 4) Deploy behavior expectation

- Root route `/` presents data science playground messaging.
- `/marketing` remains available for fallback legacy copy.
- `/notebooks/*` can host generated notebook artifacts with existing Vercel headers.
