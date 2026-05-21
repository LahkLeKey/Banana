import { useMemo, useState } from "react";
import { useNavigate } from "react-router-dom";

import {
  GAME_SESSION_STORAGE_KEY,
  rejoinGameSession,
  resolveRuntimePlayerGuid,
  resolveApiBaseResolutionError,
  resolveApiBaseUrl,
  startGameSession,
} from "../lib/api";
import { OverworldEntryCard } from "../components/session/OverworldEntryCard";

export function SessionRoomPage() {
  const navigate = useNavigate();
  const [playerName, setPlayerName] = useState("player");
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const apiBaseUrl = useMemo(() => resolveApiBaseUrl(), []);
  const resolutionError = useMemo(() => resolveApiBaseResolutionError(), []);
  const playerGuid = useMemo(() => resolveRuntimePlayerGuid(), []);

  const persistAndLaunch = (sessionId: string, token: string, nextPlayerName: string, nextPlayerGuid: string) => {
    sessionStorage.setItem(
      GAME_SESSION_STORAGE_KEY,
      JSON.stringify({
        sessionId,
        token,
        playerName: nextPlayerName,
        playerGuid: nextPlayerGuid,
      })
    );
    navigate("/game-engine");
  };

  const handleCreateSession = async () => {
    if (resolutionError) {
      setError(resolutionError);
      return;
    }

    setLoading(true);
    setError(null);
    try {
      const session = await startGameSession(apiBaseUrl, playerName.trim() || "player", playerGuid);
      persistAndLaunch(session.sessionId, session.token, session.playerName, session.playerGuid);
    } catch (cause) {
      setError(cause instanceof Error ? cause.message : String(cause));
    } finally {
      setLoading(false);
    }
  };

  const handleEnterOverworld = async () => {
    if (resolutionError) {
      setError(resolutionError);
      return;
    }

    setLoading(true);
    setError(null);
    try {
      const session = await rejoinGameSession(apiBaseUrl, playerGuid);
      persistAndLaunch(
        session.sessionId,
        session.token,
        playerName.trim() || session.playerName,
        session.playerGuid
      );
    } catch (cause) {
      setError(cause instanceof Error ? cause.message : String(cause));
    } finally {
      setLoading(false);
    }
  };

  return (
    <main
      style={{
        minHeight: "100vh",
        display: "grid",
        placeItems: "center",
        background: "linear-gradient(180deg, #020617 0%, #0f172a 100%)",
        padding: 24,
      }}
    >
      <OverworldEntryCard
        playerName={playerName}
        loading={loading}
        error={error}
        onPlayerNameChange={setPlayerName}
        onEnterOverworld={() => {
          void handleEnterOverworld();
        }}
        onResetAndEnter={() => {
          void handleCreateSession();
        }}
      />
    </main>
  );
}
