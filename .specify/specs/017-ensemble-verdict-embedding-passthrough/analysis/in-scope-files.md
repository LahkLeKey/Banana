# In-scope files -- 017 Ensemble Verdict Embedding Passthrough

## Source under change

- `src/c-sharp/asp.net/NativeInterop/INativeBananaClient.cs`
  (new method `ClassifyBananaTransformerWithEmbedding`)
- `src/c-sharp/asp.net/NativeInterop/NativeBananaClient.cs`
  (binding to existing `banana_classify_banana_transformer_ex`)
- `src/c-sharp/asp.net/NativeInterop/NativeMethods.cs`
  (P/Invoke declaration for `_ex`, if not already present)
- `src/c-sharp/asp.net/Pipeline/Results/EnsembleVerdictWithEmbeddingResult.cs`
  (NEW)
- `src/c-sharp/asp.net/Controllers/BananaMlController.cs`
  (`[HttpPost("ensemble/embedding")]` route)
- `src/c-sharp/asp.net/Program.cs` (DI for any new helper if added)

## Tests under change

- `tests/unit/TestDoubles/FakeNativeBananaClient.cs`
  (extend with embedding-aware fake)
- `tests/unit/EnsembleEmbeddingPassthroughTests.cs` (NEW; 6 anchor walks)
- `tests/unit/BananaMlControllerTests.cs` (no signature change; may add
  one snapshot lock for `/ml/ensemble`)

## Out of scope

- `src/native/**` (no exports added; ABI 2.2 preserved)
- `src/typescript/**` (React consumption is a future slice)
- New PostgreSQL queries or compose/runtime work
