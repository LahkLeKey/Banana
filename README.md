#### Pass the Banana from C into various different Applications. (APIs, Desktop, Frontend, Mobile, etc.)
------------------

## Local Runtime Launch (VS Code) + Docker Desktop

- Use Run and Debug profile `Banana Channels (Container Driven)` from `.vscode/launch.json`.
- This one-click profile launches all Banana channels through Docker Compose, including the Electron desktop window via WSL2/WSLg forwarding.
- Expected local endpoints after startup:
   - API health: `http://localhost:8080/health`
   - React app: `http://localhost:5173`
   - React Native web: `http://localhost:19006`
- Requirements for desktop window channel:
   - WSL2 with WSLg enabled
   - Docker Desktop with WSL integration enabled
- To stop containers, run `bash scripts/compose-apps-down.sh` (or the `Stop Compose Apps` task).

> See the [Wiki](https://github.com/LahkLeKey/Banana/wiki) for more details

> Prototype for "Poly", a polymorphic multi-platform runtime for native code, starting with C and expanding to other languages as needed. The goal is to enable core logic in C to be reused across different application types while maintaining performance and consistency.