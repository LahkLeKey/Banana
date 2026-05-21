# Tasks: Overworld Runtime Follow-up

**Input**: `.specify/specs/054-overworld-runtime-followup/spec.md`

## Implementation

- [x] T001 Extract reusable session entry component with typed props interface in `src/typescript/react/src/components/session/OverworldEntryCard.tsx` and wire `src/typescript/react/src/pages/SessionRoomPage.tsx`
- [x] T002 Extract reusable gameplay HUD composition component with typed props interface in `src/typescript/react/src/components/game/OverworldHud.tsx` and wire `src/typescript/react/src/pages/GameEnginePage.tsx`
- [ ] T003 Add tab-local runtime identity derivation to support two simultaneous local overworld clients without GUID collision in `src/typescript/react/src/lib/api.ts` and `src/typescript/react/src/pages/SessionRoomPage.tsx`
- [ ] T004 Extract websocket/session orchestration from `src/typescript/react/src/pages/GameEnginePage.tsx` into a reusable typed hook module under `src/typescript/react/src/hooks/`
- [ ] T005 Validate runtime behavior with two local clients in shared overworld and capture exact observed behavior

## Validation

- [ ] V001 `bun run --cwd src/typescript/react build`
- [ ] V002 `bun test src/typescript/api/src/routes/game-session.test.ts`
- [ ] V003 Two-client browser smoke on `/session-room` -> `/game-engine` with shared overworld visibility evidence
