# Corpus Curation Notes

## Curation rubric

- Keep labels strictly in `{banana, not-banana}`.
- Use human-readable, concrete text examples rather than generated filler.
- Maintain at least 8 samples per class and at least 20 total samples.
- Ensure negative-domain breadth across:
  - tools
  - electronics
  - household items
  - logistics/shipping
  - weather bulletins
  - document/legal language

## Anti-slop checks

- Exact duplicate sample text: blocked.
- Near-duplicate sample text: blocked at similarity ratio >= 0.93.
- Empty, malformed, or schema-incompatible sample fields: blocked.

## Validation command

```bash
python scripts/check-corpus-schema.py data/not-banana/corpus.json
```
