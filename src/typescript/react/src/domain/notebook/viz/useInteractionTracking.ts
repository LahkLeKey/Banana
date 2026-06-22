import {useCallback, useEffect, useRef, useState} from 'react';

import {type K3h4RecordEpochRequest, type K3h4TrainingMode, recordK3h4TrainingEpoch, resolveApiBaseUrl} from '../../../lib/api';
import type {NetcodeK3h4Payload} from '../../../lib/api';

export type InteractionKind = 'bootstrap'|'relations'|'expand-details'|
    'collapse-details'|'workflow-step'|'model-query';

export type TrackedInteraction = {
  readonly kind: InteractionKind;
  readonly timestamp: number;
  readonly impactScore: number; // 0-1: how significant was this interaction
  readonly contextMetrics?: {
    readonly confidence?: number;
    readonly alignment?: number;
    readonly variance?: number;
    readonly flow?: number;
    readonly clusterCount?: number;
  };
};

export type InteractionMetrics = {
  readonly totalInteractions: number; readonly bootstrapCount: number; readonly relationsCount: number; readonly xpEarned: number; readonly level: number; readonly interactionStreak:
                                                                                                                                                                        number;  // consecutive sessions with interactions
};

type InteractionTrackingConfig = {
  readonly sessionId: string;
  readonly mode: K3h4TrainingMode;
  readonly k3h4Payload?: NetcodeK3h4Payload;
  readonly confidence?: number;
  readonly alignment?: number;
  readonly radialStability?: number;
};

/**
 * Converts interaction patterns into K3H4 training data.
 * Maps UI interactions to reinforcement signals that feed the ML model.
 */
export function useInteractionTracking(config: InteractionTrackingConfig) {
  const [metrics, setMetrics] = useState<InteractionMetrics>({
    totalInteractions: 0,
    bootstrapCount: 0,
    relationsCount: 0,
    xpEarned: 0,
    level: 1,
    interactionStreak: 0,
  });

  const [recentInteractions, setRecentInteractions] =
      useState<readonly TrackedInteraction[]>([]);
  const [pendingEpochRecording, setPendingEpochRecording] = useState(false);
  const recordingTimeoutRef = useRef<number|null>(null);

  // Calculate XP based on interaction kind and impact
  const calculateXpReward =
      useCallback((interaction: TrackedInteraction): number => {
        const baseXp: Record<InteractionKind, number> = {
          bootstrap: 50,
          relations: 75,
          'expand-details': 10,
          'collapse-details': 5,
          'workflow-step': 25,
          'model-query': 100,
        };

        const base = baseXp[interaction.kind];
        const impactBonus = Math.floor(interaction.impactScore * 50);
        return base + impactBonus;
      }, []);

  // Calculate level based on total XP
  const calculateLevel = useCallback((xp: number): number => {
    // XP formula: each level requires (100 + level*50) XP
    // Level 1 = 0-100, Level 2 = 100-150, Level 3 = 150-200, etc.
    let level = 1;
    let xpThreshold = 0;
    const xpPerLevel = (lv: number) => 100 + lv * 50;

    while (xpThreshold + xpPerLevel(level) <= xp && level < 99) {
      xpThreshold += xpPerLevel(level);
      level += 1;
    }
    return level;
  }, []);

  // Track an interaction and queue it for training epoch recording
  const trackInteraction = useCallback(
      (
          kind: InteractionKind,
          impactScore: number = 0.5,
          contextMetrics?: TrackedInteraction['contextMetrics'],
          ) => {
        const interaction: TrackedInteraction = {
          kind,
          timestamp: Date.now(),
          impactScore,
          contextMetrics,
        };

        setRecentInteractions((prev) => {
          const updated = [...prev, interaction];
          // Keep only last 20 interactions
          return updated.slice(-20);
        });

        // Update metrics
        setMetrics((prev) => {
          const xpGain = calculateXpReward(interaction);
          const newXp = prev.xpEarned + xpGain;
          const newLevel = calculateLevel(newXp);

          const updatedMetrics: InteractionMetrics = {
            totalInteractions: prev.totalInteractions + 1,
            bootstrapCount: kind === 'bootstrap' ? prev.bootstrapCount + 1 :
                                                   prev.bootstrapCount,
            relationsCount: kind === 'relations' ? prev.relationsCount + 1 :
                                                   prev.relationsCount,
            xpEarned: newXp,
            level: newLevel,
            interactionStreak: prev.interactionStreak + 1,
          };

          return updatedMetrics;
        });

        // Queue epoch recording (debounce to batch multiple interactions)
        if (recordingTimeoutRef.current) {
          clearTimeout(recordingTimeoutRef.current);
        }

        recordingTimeoutRef.current = window.setTimeout(() => {
          void recordTrainingEpoch();
        }, 1000);  // Batch interactions within 1 second
      },
      [calculateXpReward, calculateLevel]);

  // Construct K3H4 training data from interactions and geometry
  const buildEpochPayload = useCallback((): K3h4RecordEpochRequest|null => {
    if (!config.k3h4Payload) return null;

    const {k3h4Payload, confidence = 0, alignment = 0, radialStability = 0} =
        config;

    // Calculate reinforcement signals from recent interactions
    const interactionSignal = recentInteractions.reduce((sum, int) => {
      const kindWeight: Record<InteractionKind, number> = {
        bootstrap: 0.8,
        relations: 0.9,
        'expand-details': 0.3,
        'collapse-details': 0.2,
        'workflow-step': 0.5,
        'model-query': 1.0,
      };
      return sum + (kindWeight[int.kind] * int.impactScore);
    }, 0) / Math.max(1, recentInteractions.length);

    return {
      mode: config.mode,
      confidence: Math.min(1, confidence + interactionSignal * 0.1),
      analytics: {
        k3h4Projection: {
          alignment: Math.min(1, alignment + interactionSignal * 0.05),
          radialStability,
          nodes: k3h4Payload.centers.slice(0, 5).map(
              (_, i) => ({
                x: Math.sin((i / 5) * Math.PI * 2),
                y: Math.cos((i / 5) * Math.PI * 2),
              })),
        },
        k3h4: {
          centers: k3h4Payload.centers.map((c) => ({clusterId: c.clusterId})),
          radii:
              k3h4Payload.radii.map((r) => ({
                                      clusterId: r.clusterId,
                                      inscribedRadiusQ16: r.inscribedRadiusQ16,
                                    })),
          weightedVoronoiScores: k3h4Payload.weightedVoronoiScores.map(
              (s) => ({
                clusterId: s.clusterId,
                weightedScoreQ16: s.weightedScoreQ16,
              })),
          spectralProxy: k3h4Payload.spectralProxy.map(
              (p) => ({
                clusterId: p.clusterId,
                amplitudeProxyQ16: p.amplitudeProxyQ16,
              })),
        },
        k3h4Runtime: {
          spectralActivation: 'disabled' as const,
        },
      },
    };
  }, [config, recentInteractions]);

  // Record training epoch with reinforcement data
  const recordTrainingEpoch = useCallback(async () => {
    if (pendingEpochRecording || !config.sessionId) return;

    setPendingEpochRecording(true);
    try {
      const payload = buildEpochPayload();
      if (!payload) return;

      const baseUrl = resolveApiBaseUrl();
      await recordK3h4TrainingEpoch(baseUrl, config.sessionId, payload);

      // Clear recent interactions after successful recording
      setRecentInteractions([]);
    } catch (error) {
      console.warn('Failed to record training epoch:', error);
    } finally {
      setPendingEpochRecording(false);
    }
  }, [config.sessionId, buildEpochPayload, pendingEpochRecording]);

  // Cleanup timeout on unmount
  useEffect(() => {
    return () => {
      if (recordingTimeoutRef.current) {
        clearTimeout(recordingTimeoutRef.current);
      }
    };
  }, []);

  return {
    metrics,
    recentInteractions,
    trackInteraction,
    recordTrainingEpoch,
    pendingEpochRecording,
  };
}
