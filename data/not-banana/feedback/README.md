# Not-Banana Feedback Inbox

This folder stores classifier feedback entries that can be promoted into
`data/not-banana/corpus.json` through automation.

## File

- `inbox.json`: queue of feedback entries.

## Entry Shape

Each entry in `inbox.json` uses this shape:

- `id`: unique feedback identifier.
- `status`: lifecycle state (`pending`, `approved`, `rejected`, `applied`, `error`).
- `label`: `BANANA` or `NOT_BANANA`.
- `payload`: request payload object matching `/not-banana/junk` polymorphic shape.
- `source`: optional source channel (`ui`, `api`, `manual`, etc.).
- `notes`: optional reviewer notes.
- `captured_at_utc`: optional RFC3339 UTC timestamp.

Only entries whose `status` matches the selected status filter (default:
`approved`) are processed by automation.

## Automation Flow

1. Add or update entries in `inbox.json`.
2. Run workflow `Orchestrate Not-Banana Feedback Loop`.
3. Workflow opens an automation PR with corpus updates and feedback status updates.
4. After merge, training workflow runs on push and persists registry history for
   corpus changes.
