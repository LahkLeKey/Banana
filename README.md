#### Pass the Banana from C into various different Applications. (APIs, Desktop, Frontend, Mobile, etc.)
------------------

> Read the [Wiki](https://github.com/LahkLeKey/Banana/wiki) for more details

<img width="2331" height="1890" alt="image" src="https://github.com/user-attachments/assets/509b39d6-8588-4e22-bfd9-c2c658f58e9a" />

## Compose Workflows

- Boot the API, PostgreSQL, native build step, and Electron QA client together: `bash scripts/compose-apps.sh`
- The apps profile now also starts the React UI at http://localhost:5173
- Boot the same application stack in detached mode: `DETACH=1 bash scripts/compose-apps.sh`
- Stop the application stack: `bash scripts/compose-apps-down.sh`
