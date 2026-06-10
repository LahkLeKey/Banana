# Quickstart: Notebook MMO Gameplay Client

## 1) Generate notebook gameplay artifacts

```bash
bash scripts/scaffold-abi-notebook-workflow.sh
```

Expected outputs:

- `notebooks/native-c-catalog.ipynb`
- `notebooks/catalog-index.json`
- `src/typescript/react/public/notebooks/*`

## 2) Start React gameplay client shell

```bash
cd src/typescript/react
bun run dev --host 0.0.0.0 --port 5173
```

Open `http://localhost:5173/`.

## 3) Validate notebook client endpoints

- `http://localhost:5173/notebooks/catalog-index.json`
- `http://localhost:5173/notebooks/native-c-catalog.ipynb`

## 4) Build gate

```bash
cd src/typescript/react
bun run build
```

## 5) Native baseline safety gate (optional for this feature branch)

```bash
cmake --build out/v3-native -- -m:1
ctest -C Debug --test-dir out/v3-native -R "banana_native_feedback_loop_factory_(smoke|dx12_scaffold_smoke|negotiate_smoke|comeback_smoke|flank_smoke)" --output-on-failure
```
