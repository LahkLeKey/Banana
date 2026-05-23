import type {MutableRefObject} from 'react';

import type {NetcodeDomain} from '../domains/NetcodeDomain';
import type {PredictionDomain} from '../domains/PredictionDomain';
import type {ReplicationDomain} from '../domains/ReplicationDomain';

import {type GameSessionBootstrap, type GameSessionPlayer, type GameSessionSnapshotEnvelope, resolveApiBaseUrl, resolveGameSessionWebSocketUrl,} from './api';
import type {BananaEngineModule, WorldPosition} from './types';

export type SessionSocketContext = {
  sessionBootstrap: GameSessionBootstrap;
  moduleRef: MutableRefObject<BananaEngineModule|null>;
  websocketRef: MutableRefObject<WebSocket|null>;
  outboundSequenceRef: MutableRefObject<number>;
  outboundTickRef: MutableRefObject<number>;
  localPlayerIdRef: MutableRefObject<string|null>;
  renderedRemotePlayerIdsRef: MutableRefObject<Set<string>>;
  predictionRef: MutableRefObject<PredictionDomain|null>;
  netcodeRef: MutableRefObject<NetcodeDomain|null>;
  replicationRef: MutableRefObject<ReplicationDomain|null>;
  setError: (value: string|null) => void;
  setLocalPlayerWorldPosition: (value: WorldPosition|null) => void;
  applySessionPlayers: (players: GameSessionPlayer[]) => void;
};

export function attachGameSessionSocket(context: SessionSocketContext): () =>
    void {
  const {
    sessionBootstrap,
    moduleRef,
    websocketRef,
    outboundSequenceRef,
    outboundTickRef,
    localPlayerIdRef,
    renderedRemotePlayerIdsRef,
    predictionRef,
    netcodeRef,
    replicationRef,
    setError,
    setLocalPlayerWorldPosition,
    applySessionPlayers,
  } = context;

  const websocketUrl = resolveGameSessionWebSocketUrl(
      resolveApiBaseUrl(), sessionBootstrap.sessionId,
      sessionBootstrap.playerName, sessionBootstrap.playerGuid);
  const socket = new WebSocket(websocketUrl);

  websocketRef.current = socket;

  socket.addEventListener('close', () => {
    if (websocketRef.current === socket) {
      websocketRef.current = null;
    }
  });

  socket.addEventListener('error', () => {
    setError('Realtime session connection failed. Rejoin the room and retry.');
  });

  socket.addEventListener('message', (event) => {
    if (typeof event.data !== 'string') {
      return;
    }

    try {
      const payload =
          JSON.parse(event.data) as | GameSessionSnapshotEnvelope | {
        type: 'connected';
        playerId?: string;
        players?: GameSessionPlayer[];
        lastSequence?: number;
        lastTick?: number;
      }
      |{
        type: 'ack';
        sequence?: number;
        tick?: number
      }
      |{
        type: 'pong';
        ts?: number
      }
      |{
        type: 'error';
        error?: string;
        detail?: string;
        expectedSequence?: number;
        expectedTick?: number;
      };

      if (payload.type === 'connected') {
        localPlayerIdRef.current = payload.playerId ?? null;
        if (Number.isFinite(payload.lastSequence)) {
          outboundSequenceRef.current = Math.max(
              outboundSequenceRef.current, Number(payload.lastSequence));
        }
        if (Number.isFinite(payload.lastTick)) {
          outboundTickRef.current =
              Math.max(outboundTickRef.current, Number(payload.lastTick));
        }
        applySessionPlayers(payload.players ?? []);
        return;
      }

      if (payload.type === 'error') {
        const detail = payload.detail ?? 'Realtime session reported an error.';
        if (payload.error === 'invalid_input') {
          const nextSequence =
              Number(payload.expectedSequence ?? outboundSequenceRef.current);
          const nextTick =
              Number(payload.expectedTick ?? outboundTickRef.current);
          if (Number.isFinite(nextSequence)) {
            outboundSequenceRef.current =
                Math.max(outboundSequenceRef.current, nextSequence);
          }
          if (Number.isFinite(nextTick)) {
            outboundTickRef.current =
                Math.max(outboundTickRef.current, nextTick);
          }
          return;
        }

        setError(detail);
        return;
      }

      if (payload.type === 'ack' || payload.type === 'pong') {
        return;
      }

      if (payload.type !== 'snapshot') {
        return;
      }

      applySessionPlayers(payload.players ?? []);
      const module = moduleRef.current;
      if (module) {
        const remotePlayerIds = new Set<string>();
        const playersById = new Map(
            (payload.players ?? []).map((player) => [player.playerId, player]));

        Object.values(payload.snapshot.entities ?? {}).forEach((entity) => {
          if (entity.type !== 'player') return;
          const playerId = (entity.playerGuid ?? entity.ownerPlayerId ?? '')
                               .trim()
                               .toLowerCase();
          if (!playerId) return;
          if (playerId === localPlayerIdRef.current) return;

          remotePlayerIds.add(playerId);
          const rosterPlayer = playersById.get(playerId);

          module.ccall(
              'engine_player_upsert', 'number',
              ['string', 'string', 'string', 'number'], [
                playerId,
                entity.ownerPlayerName ?? rosterPlayer?.playerName ?? playerId,
                entity.controllerKind ?? rosterPlayer?.controllerKind ??
                    'human',
                entity.active ? 1 : 0,
              ]);

          module.ccall(
              'engine_player_set_transform', 'null',
              ['string', 'number', 'number', 'number', 'number'],
              [playerId, entity.x, entity.y, entity.z, entity.active ? 1 : 0]);
        });

        renderedRemotePlayerIdsRef.current.forEach((playerId) => {
          if (remotePlayerIds.has(playerId)) return;
          module.ccall(
              'engine_player_upsert', 'number',
              ['string', 'string', 'string', 'number'],
              [playerId, playerId, 'human', 0]);
        });

        renderedRemotePlayerIdsRef.current = remotePlayerIds;
      }

      replicationRef.current?.applyAuthoritativeState(payload.snapshot);

      const localPlayer = (localPlayerIdRef.current ?
                               Object.values(payload.snapshot.entities)
                                   .find(
                                       (entity) => entity.ownerPlayerId ===
                                           localPlayerIdRef.current) :
                               undefined) ??
          payload.snapshot.entities['entity-1'] ??
          Object.values(payload.snapshot.entities)
              .find((entity) => entity.type === 'player');

      if (!localPlayer) {
        return;
      }

      setLocalPlayerWorldPosition({x: localPlayer.x, z: localPlayer.z});

      predictionRef.current?.reconcileAndReplay({
        tickIndex: payload.snapshot.tick,
        positionX: localPlayer.x,
        positionZ: localPlayer.z,
        timestamp: payload.snapshot.timestamp,
      });

      netcodeRef.current?.reconcileAuthoritativeState(payload.snapshot.tick, {
        tickIndex: payload.snapshot.tick,
        moveX: 0,
        moveZ: 0,
        timestamp: payload.snapshot.timestamp,
        isAuthoritative: true,
      });
    } catch {
      // Ignore malformed snapshot payloads and wait for the next frame.
    }
  });

  return () => {
    if (websocketRef.current === socket) {
      websocketRef.current = null;
    }
    renderedRemotePlayerIdsRef.current = new Set();
    socket.close();
  };
}