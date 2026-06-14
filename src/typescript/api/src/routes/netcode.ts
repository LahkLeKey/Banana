import type {FastifyInstance, FastifyReply} from 'fastify';

import {createK3h4ApplicationOrchestrationLayer, type K3h4ApplicationOrchestrationLayer,} from '../services/k3h4ApplicationOrchestrationLayer.ts';
import {getNativeNetcodeService, type NativeNetcodeService,} from '../services/nativeNetcode.ts';
import {createNetcodeAnalyticsAuthoritativeComputeOrchestrator, type NetcodeAnalyticsAuthoritativeComputeOrchestrator, NetcodeAnalyticsOrchestrationError, type NetcodeK3h4Rollout,} from '../services/netcodeAuthoritativeComputeOrchestrator.ts';

type NetcodeRouteOptions = {
  netcodeAuthoritativeComputeOrchestrator?:
      NetcodeAnalyticsAuthoritativeComputeOrchestrator;
  k3h4ApplicationOrchestrationLayer?: K3h4ApplicationOrchestrationLayer;
  netcodeService?: NativeNetcodeService;
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
    options: NetcodeRouteOptions = {},
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
}
