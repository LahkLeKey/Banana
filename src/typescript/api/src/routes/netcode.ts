import type {FastifyInstance, FastifyReply} from 'fastify';

import {createK3h4ApplicationOrchestrationLayer, type K3h4ApplicationOrchestrationLayer,} from '../services/k3h4ApplicationOrchestrationLayer.ts';
import {loadK3h4ScalingBenchmark} from '../services/k3h4ScalingBenchmark.ts';
import type {K3h4ScalingBenchmarkStatus} from '../services/k3h4ScalingBenchmark.ts';
import {createFileSystemK3h4TrainingService, type K3h4TrainingService, K3h4VizServiceError, type TrainingSessionMode,} from '../services/k3h4TrainingService.ts';
import {getNativeNetcodeService, type NativeNetcodeService,} from '../services/nativeNetcode.ts';
import {createNetcodeAnalyticsAuthoritativeComputeOrchestrator, type NetcodeAnalyticsAuthoritativeComputeOrchestrator, NetcodeAnalyticsOrchestrationError, type NetcodeK3h4Rollout,} from '../services/netcodeAuthoritativeComputeOrchestrator.ts';

type NetcodeRouteOptions = {
  netcodeAuthoritativeComputeOrchestrator?:
      NetcodeAnalyticsAuthoritativeComputeOrchestrator;
  k3h4ApplicationOrchestrationLayer?: K3h4ApplicationOrchestrationLayer;
  netcodeService?: NativeNetcodeService;
};

type NetcodeRouteOptionsExtended = NetcodeRouteOptions&{
  scalingBenchmarkLoader?: () => K3h4ScalingBenchmarkStatus;
  k3h4TrainingService?: K3h4TrainingService;
};

function isFiniteNumber(value: unknown): value is number {
  return typeof value === 'number' && Number.isFinite(value);
}

function isValidWorkflowDepth(value: unknown): value is 1|2|3 {
  return isFiniteNumber(value) && (value === 1 || value === 2 || value === 3);
}

function isValidNetworkDimensions(value: unknown): boolean {
  return isFiniteNumber(value) && Number.isInteger(value) && value >= 1 &&
      value <= 16;
}

function isValidK3h4Mode(value: unknown): value is 'multiplicative'|'power' {
  return value === 'multiplicative' || value === 'power';
}

function isValidSpectralMode(value: unknown): value is
    'disabled'|'affinity-graph' {
  return value === 'disabled' || value === 'affinity-graph';
}

function parseTrainingMode(rawMode: unknown): TrainingSessionMode|null {
  if (rawMode === undefined || rawMode === null || rawMode === '') {
    return 'power';
  }
  if (rawMode === 'multiplicative' || rawMode === 'power') {
    return rawMode;
  }
  return null;
}

function parseBulkSessionIds(rawValue: unknown): string[]|null {
  if (!Array.isArray(rawValue) || rawValue.length < 1 ||
      rawValue.length > 128) {
    return null;
  }

  const normalized: string[] = [];
  for (const item of rawValue) {
    if (typeof item !== 'string') {
      return null;
    }
    const trimmed = item.trim();
    if (trimmed.length < 1 || trimmed.length > 64) {
      return null;
    }
    normalized.push(trimmed);
  }

  return normalized;
}

type RecordTrainingEpochPayload = {
  mode?: TrainingSessionMode;
  confidence?: number;
  analytics?: {
    k3h4Projection?: {
      alignment?: number;
      radialStability?: number;
      nodes?: Array<{x?: number; y?: number}>;
    };
    k3h4?: {
      centers?: Array<{clusterId?: number}>;
      radii?: Array<{clusterId?: number; inscribedRadiusQ16?: number}>;
      weightedVoronoiScores?:
          Array<{clusterId?: number; weightedScoreQ16?: number}>;
      spectralProxy?: Array<{clusterId?: number; amplitudeProxyQ16?: number}>;
    };
    k3h4Runtime?: {spectralActivation?: 'disabled' | 'affinity-graph';};
  };
};

function isRecordTrainingEpochPayload(body: unknown):
    body is Required<RecordTrainingEpochPayload> {
  if (!body || typeof body !== 'object') {
    return false;
  }

  const payload = body as RecordTrainingEpochPayload;
  if (!isFiniteNumber(payload.confidence) || !payload.analytics) {
    return false;
  }

  const projection = payload.analytics.k3h4Projection;
  const k3h4 = payload.analytics.k3h4;
  const runtime = payload.analytics.k3h4Runtime;

  if (!projection || !k3h4 || !runtime) {
    return false;
  }

  if (!isFiniteNumber(projection.alignment) ||
      !isFiniteNumber(projection.radialStability) ||
      !Array.isArray(projection.nodes) || !Array.isArray(k3h4.centers) ||
      !Array.isArray(k3h4.radii) ||
      !Array.isArray(k3h4.weightedVoronoiScores) ||
      !Array.isArray(k3h4.spectralProxy) ||
      (runtime.spectralActivation !== 'disabled' &&
       runtime.spectralActivation !== 'affinity-graph')) {
    return false;
  }

  return true;
}

function resolveNetcodeK3h4Rollout(): NetcodeK3h4Rollout {
  const enabledRaw =
      (process.env.BANANA_NETCODE_K3H4_ENABLED ?? 'true').trim().toLowerCase();
  const enabled =
      enabledRaw !== 'false' && enabledRaw !== '0' && enabledRaw !== 'off';
  const cohort =
      (process.env.BANANA_NETCODE_K3H4_COHORT ?? 'all').trim() || 'all';
  return {enabled, cohort};
}

function createDirectRouteRollout(cohort: string): NetcodeK3h4Rollout {
  return {
    enabled: true,
    cohort,
  };
}

type NetcodeOrchestratedComputePayload = {
  callDensity: number; questPercent: number; playerLevel: number;
  comboStreak: number;
  branchPressure: number;
  dependencyPulse: number;
  workflowDepth: 1 | 2 | 3;
  networkDimensions: number;
  modelConfidence: number;
  policyMomentum: number;
  k3h4Mode?: 'multiplicative' | 'power';
  spectralMode?: 'disabled' | 'affinity-graph';
  neuralRelevanceScore?: number;
};

function isValidOrchestratedComputePayload(
    body: NetcodeOrchestratedComputePayload|undefined,
    requireNeuralRelevanceScore: boolean,
    ): boolean {
  if (!body || !isFiniteNumber(body.callDensity) ||
      !isFiniteNumber(body.questPercent) || !isFiniteNumber(body.playerLevel) ||
      !isFiniteNumber(body.comboStreak) ||
      !isFiniteNumber(body.branchPressure) ||
      !isFiniteNumber(body.dependencyPulse) ||
      !isValidWorkflowDepth(body.workflowDepth) ||
      !isValidNetworkDimensions(body.networkDimensions) ||
      !isFiniteNumber(body.modelConfidence) ||
      !isFiniteNumber(body.policyMomentum) ||
      (body.k3h4Mode !== undefined && !isValidK3h4Mode(body.k3h4Mode)) ||
      (body.spectralMode !== undefined &&
       !isValidSpectralMode(body.spectralMode))) {
    return false;
  }

  if (requireNeuralRelevanceScore &&
      !isFiniteNumber(body.neuralRelevanceScore)) {
    return false;
  }

  return true;
}

export async function registerNetcodeRoutes(
    app: FastifyInstance,
    options: NetcodeRouteOptionsExtended = {},
    ): Promise<void> {
  const netcode = options.netcodeService ?? getNativeNetcodeService();
  const netcodeAuthoritativeComputeOrchestrator =
      options.netcodeAuthoritativeComputeOrchestrator ??
      createNetcodeAnalyticsAuthoritativeComputeOrchestrator(netcode);
  const k3h4ApplicationOrchestrationLayer =
      options.k3h4ApplicationOrchestrationLayer ??
      (options.netcodeAuthoritativeComputeOrchestrator ?
           {
             compute: (request, rollout) =>
                 netcodeAuthoritativeComputeOrchestrator.compute(
                     request, rollout),
           } :
           createK3h4ApplicationOrchestrationLayer(netcode));

  const benchmarkLoader =
      options.scalingBenchmarkLoader ?? loadK3h4ScalingBenchmark;
  const trainingService =
      options.k3h4TrainingService ?? createFileSystemK3h4TrainingService();

  type OrchestratedNetcodeResult =
      Awaited<ReturnType<K3h4ApplicationOrchestrationLayer['compute']>>;

  async function computeOrSendOrchestrationError(
      computeRequest:
          Parameters<K3h4ApplicationOrchestrationLayer['compute']>[0],
      rollout: NetcodeK3h4Rollout,
      reply: FastifyReply,
      ): Promise<OrchestratedNetcodeResult|null> {
    try {
      return await k3h4ApplicationOrchestrationLayer.compute(
          computeRequest, rollout);
    } catch (error) {
      if (error instanceof NetcodeAnalyticsOrchestrationError) {
        reply.status(502).send({
          errorCode: error.errorCode,
          message: error.message,
          contractVersion: 1,
          retryable: error.retryable,
          rollout,
        });
        return null;
      }
      throw error;
    }
  }

  app.post<{
    Body: {
      callDensity: number; questPercent: number; comboStreak: number;
      branchPressure: number;
      workflowDepth: 1 | 2 | 3;
      nodeTap?: number;
      action?: number;
    }
  }>(
      '/api/netcode/learning',
      async (request, reply) => {
        const body = request.body;
        if (!body || !isFiniteNumber(body.callDensity) ||
            !isFiniteNumber(body.questPercent) ||
            !isFiniteNumber(body.comboStreak) ||
            !isFiniteNumber(body.branchPressure) ||
            !isValidWorkflowDepth(body.workflowDepth)) {
          return reply.status(400).send({
            error: 'Invalid netcode learning payload',
          });
        }

        if (typeof body.nodeTap === 'number') {
          await netcode.recordNodeTap(body.nodeTap);
        }
        if (typeof body.action === 'number') {
          await netcode.recordAction(body.action);
        }

        const ledger = await netcode.getLedger();
        const learning = await netcode.buildLearning({
          callDensity: body.callDensity,
          questPercent: body.questPercent,
          comboStreak: body.comboStreak,
          branchPressure: body.branchPressure,
          workflowDepth: body.workflowDepth,
        });

        return {
          ledger,
          learning,
        };
      },
  );

  app.post('/api/netcode/reset', async () => {
    await netcode.reset();
    return {status: 'ok'};
  });

  app.post<{
    Body: {
      callDensity: number; questPercent: number; comboStreak: number;
      branchPressure: number;
      workflowDepth: 1 | 2 | 3;
      interactionSignal?: number;
    }
  }>('/api/netcode/reward', async (request, reply) => {
    const body = request.body;
    if (!body || !isFiniteNumber(body.callDensity) ||
        !isFiniteNumber(body.questPercent) ||
        !isFiniteNumber(body.comboStreak) ||
        !isFiniteNumber(body.branchPressure) ||
        !isValidWorkflowDepth(body.workflowDepth)) {
      return reply.status(400).send({error: 'Invalid netcode reward payload'});
    }

    const reward = await netcode.buildReward(
        {
          callDensity: body.callDensity,
          questPercent: body.questPercent,
          comboStreak: body.comboStreak,
          branchPressure: body.branchPressure,
          workflowDepth: body.workflowDepth,
        },
        typeof body.interactionSignal === 'number' ? body.interactionSignal :
                                                     0);
    return {reward};
  });

  app.post<{
    Body: {
      callDensity: number; questPercent: number; playerLevel: number;
      comboStreak: number;
      branchPressure: number;
      dependencyPulse: number;
      interactionSignal?: number;
    }
  }>('/api/netcode/link', async (request, reply) => {
    const body = request.body;
    if (!body || !isFiniteNumber(body.callDensity) ||
        !isFiniteNumber(body.questPercent) ||
        !isFiniteNumber(body.playerLevel) ||
        !isFiniteNumber(body.comboStreak) ||
        !isFiniteNumber(body.branchPressure) ||
        !isFiniteNumber(body.dependencyPulse)) {
      return reply.status(400).send({error: 'Invalid netcode link payload'});
    }

    const link = await netcode.buildLink({
      callDensity: body.callDensity,
      questPercent: body.questPercent,
      playerLevel: body.playerLevel,
      comboStreak: body.comboStreak,
      branchPressure: body.branchPressure,
      dependencyPulse: body.dependencyPulse,
      interactionSignal: typeof body.interactionSignal === 'number' ?
          body.interactionSignal :
          0,
    });
    return {link};
  });

  app.post<{
    Body: {
      callDensity: number; questPercent: number; playerLevel: number;
      comboStreak: number;
      branchPressure: number;
      dependencyPulse: number;
      workflowDepth: 1 | 2 | 3;
      networkDimensions: number;
      modelConfidence: number;
      policyMomentum: number;
      interactionSignal?: number;
      k3h4Mode?: 'multiplicative' | 'power';
      spectralMode?: 'disabled' | 'affinity-graph';
    }
  }>('/api/netcode/analytics', async (request, reply) => {
    const rollout = resolveNetcodeK3h4Rollout();
    if (!rollout.enabled) {
      return reply.status(503).send({
        error: 'Netcode k3h4 analytics rollout disabled',
        rollout,
      });
    }

    const body = request.body;
    if (!isValidOrchestratedComputePayload(body, false)) {
      return reply.status(400).send(
          {error: 'Invalid netcode analytics payload'});
    }

    const orchestratedAnalytics =
        await computeOrSendOrchestrationError(body, rollout, reply);
    if (!orchestratedAnalytics) return;

    return {...orchestratedAnalytics, rollout};
  });

  app.post<{
    Body: {
      callDensity: number; questPercent: number; playerLevel: number;
      comboStreak: number;
      branchPressure: number;
      dependencyPulse: number;
      workflowDepth: 1 | 2 | 3;
      neuralRelevanceScore: number;
      networkDimensions: number;
      modelConfidence: number;
      policyMomentum: number;
      k3h4Mode?: 'multiplicative' | 'power';
      spectralMode?: 'disabled' | 'affinity-graph';
    }
  }>('/api/netcode/vector', async (request, reply) => {
    const body = request.body;
    if (!isValidOrchestratedComputePayload(body, true)) {
      return reply.status(400).send({error: 'Invalid netcode vector payload'});
    }

    const directVectorRollout = createDirectRouteRollout('vector-direct-route');

    const orchestratedAnalytics =
        await computeOrSendOrchestrationError(body, directVectorRollout, reply);
    if (!orchestratedAnalytics) return;

    const vector = orchestratedAnalytics.vector;
    return {vector};
  });

  app.post<{
    Body: {
      callDensity: number; questPercent: number; playerLevel: number;
      comboStreak: number;
      branchPressure: number;
      dependencyPulse: number;
      workflowDepth: 1 | 2 | 3;
      neuralRelevanceScore: number;
      networkDimensions: number;
      modelConfidence: number;
      policyMomentum: number;
      k3h4Mode?: 'multiplicative' | 'power';
      spectralMode?: 'disabled' | 'affinity-graph';
    }
  }>('/api/netcode/k3h4', async (request, reply) => {
    const body = request.body;
    if (!isValidOrchestratedComputePayload(body, true)) {
      return reply.status(400).send({error: 'Invalid netcode k3h4 payload'});
    }

    const directK3h4Rollout = createDirectRouteRollout('k3h4-direct-route');

    const orchestratedAnalytics =
        await computeOrSendOrchestrationError(body, directK3h4Rollout, reply);
    if (!orchestratedAnalytics) return;

    return {k3h4: orchestratedAnalytics.k3h4Projection};
  });

  /* ------------------------------------------------------------------
   * GET /api/netcode/k3h4/scaling-benchmark
   *
   * Returns the measured O(n) vs O(n²) scaling series produced by the
   * native banana_k3h4_scaling_benchmark_test CTest.
   * ------------------------------------------------------------------ */
  app.get('/api/netcode/k3h4/scaling-benchmark', async (_request, reply) => {
    const status = benchmarkLoader();
    if (status.kind === 'not_found') {
      return reply.status(404).send({
        error: 'benchmark_not_found',
        message:
            'Scaling benchmark artifact not found. Run the native CTest benchmark first.',
      });
    }
    if (status.kind === 'unavailable') {
      return reply.status(503).send({
        error: 'benchmark_unavailable',
        message: `Failed to read scaling benchmark artifact: ${status.reason}`,
      });
    }
    return status.result;
  });

  app.post<{Body: {mode?: TrainingSessionMode}}>(
      '/api/netcode/k3h4/training-session', async (request, reply) => {
        const mode = parseTrainingMode(request.body?.mode);
        if (!mode) {
          return reply.status(400).send({
            error: 'invalid_mode',
            message:
                'mode must be either multiplicative or power when provided.',
          });
        }

        const session = await trainingService.createTrainingSession(mode);
        return reply.status(201).send(session);
      });

  app.post<{Params: {id: string}; Body: RecordTrainingEpochPayload}>(
      '/api/netcode/k3h4/training-session/:id/epoch',
      async (request, reply) => {
        const mode = parseTrainingMode(request.body?.mode);
        if (!mode) {
          return reply.status(400).send({
            error: 'invalid_mode',
            message:
                'mode must be either multiplicative or power when provided.',
          });
        }

        if (!isRecordTrainingEpochPayload(request.body)) {
          return reply.status(400).send({
            error: 'invalid_epoch_payload',
            message:
                'Epoch payload must include confidence and analytics snapshot fields.',
          });
        }

        if (!trainingService.recordEpochFromAnalytics) {
          return reply.status(503).send({
            error: 'training_unavailable',
            message: 'Epoch recording is not available for current runtime.',
          });
        }

        try {
          const persisted = await trainingService.recordEpochFromAnalytics(
              request.params.id,
              mode,
              request.body.confidence,
              {
                k3h4Projection: {
                  alignment: request.body.analytics.k3h4Projection.alignment,
                  radialStability:
                      request.body.analytics.k3h4Projection.radialStability,
                  nodes: request.body.analytics.k3h4Projection.nodes.map(
                      (node) => ({
                        x: isFiniteNumber(node.x) ? node.x : 0,
                        y: isFiniteNumber(node.y) ? node.y : 0,
                      })),
                },
                k3h4: {
                  centers: request.body.analytics.k3h4.centers.map(
                      (center) => ({
                        clusterId: isFiniteNumber(center.clusterId) ?
                            center.clusterId :
                            0,
                      })),
                  radii: request.body.analytics.k3h4.radii.map(
                      (radius) => ({
                        clusterId: isFiniteNumber(radius.clusterId) ?
                            radius.clusterId :
                            0,
                        inscribedRadiusQ16:
                            isFiniteNumber(radius.inscribedRadiusQ16) ?
                            radius.inscribedRadiusQ16 :
                            0,
                      })),
                  weightedVoronoiScores:
                      request.body.analytics.k3h4.weightedVoronoiScores.map(
                          (score) => ({
                            clusterId: isFiniteNumber(score.clusterId) ?
                                score.clusterId :
                                0,
                            weightedScoreQ16:
                                isFiniteNumber(score.weightedScoreQ16) ?
                                score.weightedScoreQ16 :
                                0,
                          })),
                  spectralProxy: request.body.analytics.k3h4.spectralProxy.map(
                      (spectral) => ({
                        clusterId: isFiniteNumber(spectral.clusterId) ?
                            spectral.clusterId :
                            0,
                        amplitudeProxyQ16:
                            isFiniteNumber(spectral.amplitudeProxyQ16) ?
                            spectral.amplitudeProxyQ16 :
                            0,
                      })),
                },
                k3h4Runtime: {
                  spectralActivation:
                      request.body.analytics.k3h4Runtime.spectralActivation,
                },
              },
          );
          return reply.status(201).send({
            contractVersion: 1,
            sessionId: request.params.id,
            mode,
            epochIndex: persisted.epochIndex,
            persistedAtUtc: persisted.persistedAtUtc,
          });
        } catch (error) {
          if (error instanceof K3h4VizServiceError) {
            return reply.status(error.statusCode).send({
              error: error.code,
              message: error.message,
            });
          }
          throw error;
        }
      },
  );

  app.get<{Params: {id: string}; Querystring: {mode?: string}}>(
      '/api/netcode/k3h4/training-session/:id/confidence',
      async (request, reply) => {
        const mode = parseTrainingMode(request.query.mode);
        if (!mode) {
          return reply.status(400).send({
            error: 'invalid_mode',
            message:
                'mode must be either multiplicative or power when provided.',
          });
        }

        try {
          const confidence = await trainingService.readConfidenceTimeSeries(
              request.params.id, mode);
          return reply.send(confidence);
        } catch (error) {
          if (error instanceof K3h4VizServiceError) {
            return reply.status(error.statusCode).send({
              error: error.code,
              message: error.message,
            });
          }
          throw error;
        }
      },
  );

  app.post<{Body: {mode?: string; sessionIds?: string[];}}>(
      '/api/netcode/k3h4/training-session/confidence/bulk',
      async (request, reply) => {
        const mode = parseTrainingMode(request.body?.mode);
        if (!mode) {
          return reply.status(400).send({
            error: 'invalid_mode',
            message:
                'mode must be either multiplicative or power when provided.',
          });
        }

        const requestedSessionIds =
            parseBulkSessionIds(request.body?.sessionIds);
        if (!requestedSessionIds) {
          return reply.status(400).send({
            error: 'invalid_session_ids',
            message:
                'sessionIds must be an array with 1-128 non-empty session identifiers.',
          });
        }

        const dedupedSessionIds = Array.from(new Set(requestedSessionIds));
        const results =
            await Promise.all(dedupedSessionIds.map(async (sessionId) => {
              try {
                const confidence =
                    await trainingService.readConfidenceTimeSeries(
                        sessionId, mode);
                return {
                  sessionId,
                  confidence,
                  error: null,
                };
              } catch (error) {
                if (error instanceof K3h4VizServiceError) {
                  return {
                    sessionId,
                    confidence: null,
                    error: {
                      code: error.code,
                      message: error.message,
                      statusCode: error.statusCode,
                    },
                  };
                }
                throw error;
              }
            }));

        return reply.send({
          contractVersion: 1,
          mode,
          requestedCount: requestedSessionIds.length,
          processedCount: dedupedSessionIds.length,
          results,
        });
      },
  );

  app.post<{
    Body: {
      mode?: string;
      requests?: Array<{sessionId: string; epochIndex: number}>;
    }
  }>(
      '/api/netcode/k3h4/training-session/geometry/bulk',
      {config: {rateLimit: {max: 600, timeWindow: '1 minute'}}},
      async (request, reply) => {
        const mode = parseTrainingMode(request.body?.mode);
        if (!mode) {
          return reply.status(400).send({
            error: 'invalid_mode',
            message:
                'mode must be either multiplicative or power when provided.',
          });
        }

        const rawRequests = request.body?.requests;
        if (!Array.isArray(rawRequests) || rawRequests.length < 1 ||
            rawRequests.length > 128) {
          return reply.status(400).send({
            error: 'invalid_requests',
            message:
                'requests must be an array with 1-128 {sessionId, epochIndex} entries.',
          });
        }

        for (const item of rawRequests) {
          if (!item || typeof item !== 'object' ||
              typeof item.sessionId !== 'string' ||
              item.sessionId.trim().length < 1 ||
              item.sessionId.trim().length > 64 ||
              !Number.isInteger(item.epochIndex) || item.epochIndex < 0) {
            return reply.status(400).send({
              error: 'invalid_requests',
              message:
                  'Each request entry must have a non-empty sessionId string and a non-negative integer epochIndex.',
            });
          }
        }

        const seen = new Set<string>();
        const dedupedRequests: Array<{sessionId: string; epochIndex: number}> =
            [];
        for (const item of rawRequests) {
          const key = `${item.sessionId.trim()}:${item.epochIndex}`;
          if (!seen.has(key)) {
            seen.add(key);
            dedupedRequests.push({
              sessionId: item.sessionId.trim(),
              epochIndex: item.epochIndex
            });
          }
        }

        const results = await Promise.all(
            dedupedRequests.map(async ({sessionId, epochIndex}) => {
              try {
                const geometry = await trainingService.readEpochGeometry(
                    sessionId, epochIndex, mode);
                return {sessionId, epochIndex, geometry, error: null};
              } catch (error) {
                if (error instanceof K3h4VizServiceError) {
                  return {
                    sessionId,
                    epochIndex,
                    geometry: null,
                    error: {
                      code: error.code,
                      message: error.message,
                      statusCode: error.statusCode,
                    },
                  };
                }
                throw error;
              }
            }));

        return reply.send({
          contractVersion: 1,
          mode,
          requestedCount: rawRequests.length,
          processedCount: dedupedRequests.length,
          results,
        });
      },
  );

  app.get<{Params: {id: string; n: string}; Querystring: {mode?: string}}>(
      '/api/netcode/k3h4/training-session/:id/epoch/:n/geometry',
      async (request, reply) => {
        const mode = parseTrainingMode(request.query.mode);
        if (!mode) {
          return reply.status(400).send({
            error: 'invalid_mode',
            message:
                'mode must be either multiplicative or power when provided.',
          });
        }

        const epochIndex = Number(request.params.n);
        if (!Number.isInteger(epochIndex) || epochIndex < 0 ||
            !Number.isFinite(epochIndex)) {
          return reply.status(400).send({
            error: 'invalid_epoch',
            message: 'Epoch index must be a non-negative integer.',
          });
        }

        try {
          const geometry = await trainingService.readEpochGeometry(
              request.params.id, epochIndex, mode);
          return reply.send(geometry);
        } catch (error) {
          if (error instanceof K3h4VizServiceError) {
            return reply.status(error.statusCode).send({
              error: error.code,
              message: error.message,
            });
          }
          throw error;
        }
      },
  );
}
