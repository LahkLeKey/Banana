#### Pass the Banana
------------------

> Read the [Wiki](https://github.com/LahkLeKey/Banana/wiki) for more details

## Compose Workflows

- Boot the API, PostgreSQL, native build step, and Electron QA client together: `bash scripts/compose-apps.sh`
- Boot the same application stack in detached mode: `DETACH=1 bash scripts/compose-apps.sh`
- Stop the application stack: `bash scripts/compose-apps-down.sh`
