# Deferred Registry

| Item | Reason Deferred | Trigger | Status |
|------|------------------|---------|--------|
| Replacement UI release verification | The implementation existed locally in source but the replacement surface was not yet deployed for integrated-browser evidence. | Deployment that serves the replacement Data Science page. | closed — resolved 2026-05-03 via `vercel deploy --prod --force`; live at banana.engineer/data-science |
| Production route cutover confirmation | banana.engineer/data-science was still the stale UI at audit time. | Deployment that makes the live route match the replacement React source. | closed — resolved 2026-05-03; production route confirmed live via fetch verification and DS Pyodide E2E tests |
