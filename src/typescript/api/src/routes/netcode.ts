import type {FastifyInstance} from 'fastify';

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

function resolveNetcodeK3h4Rollout(): NetcodeK3h4Rollout {
  const enabledRaw =
      (process.env.BANANA_NETCODE_K3H4_ENABLED ?? 'true').trim().toLowerCase();
  const enabled =
      enabledRaw !== 'false' && enabledRaw !== '0' && enabledRaw !== 'off';
  const cohort =
      (process.env.BANANA_NETCODE_K3H4_COHORT ?? 'all').trim() || 'all';
  return {enabled, cohort};
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
    if (!body || !isFiniteNumber(body.callDensity) ||
        !isFiniteNumber(body.questPercent) ||
        !isFiniteNumber(body.playerLevel) ||
        !isFiniteNumber(body.comboStreak) ||
        !isFiniteNumber(body.branchPressure) ||
        !isFiniteNumber(body.dependencyPulse) ||
        !isValidWorkflowDepth(body.workflowDepth) ||
        !isValidNetworkDimensions(body.networkDimensions) ||
        !isFiniteNumber(body.modelConfidence) ||
        !isFiniteNumber(body.policyMomentum)) {
      return reply.status(400).send(
          {error: 'Invalid netcode analytics payload'});
    }

    let orchestratedAnalytics;
    try {
      orchestratedAnalytics =
          await k3h4ApplicationOrchestrationLayer.compute(body, rollout);
    } catch (error) {
      if (error instanceof NetcodeAnalyticsOrchestrationError) {
        return reply.status(502).send({
          errorCode: error.errorCode,
          message: error.message,
          contractVersion: 1,
          retryable: error.retryable,
          rollout,
        });
      }
      throw error;
    }

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
    }
  }>('/api/netcode/vector', async (request, reply) => {
    const body = request.body;
    if (!body || !isFiniteNumber(body.callDensity) ||
        !isFiniteNumber(body.questPercent) ||
        !isFiniteNumber(body.playerLevel) ||
        !isFiniteNumber(body.comboStreak) ||
        !isFiniteNumber(body.branchPressure) ||
        !isFiniteNumber(body.dependencyPulse) ||
        !isValidWorkflowDepth(body.workflowDepth) ||
        !isFiniteNumber(body.neuralRelevanceScore) ||
        !isValidNetworkDimensions(body.networkDimensions) ||
        !isFiniteNumber(body.modelConfidence) ||
        !isFiniteNumber(body.policyMomentum)) {
      return reply.status(400).send({error: 'Invalid netcode vector payload'});
    }

    const vector = await netcode.buildVector({
      callDensity: body.callDensity,
      questPercent: body.questPercent,
      playerLevel: body.playerLevel,
      comboStreak: body.comboStreak,
      branchPressure: body.branchPressure,
      dependencyPulse: body.dependencyPulse,
      workflowDepth: body.workflowDepth,
      neuralRelevanceScore: body.neuralRelevanceScore,
      networkDimensions: body.networkDimensions,
      modelConfidence: body.modelConfidence,
      policyMomentum: body.policyMomentum,
    });
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
    }
  }>('/api/netcode/k3h4', async (request, reply) => {
    const body = request.body;
    if (!body || !isFiniteNumber(body.callDensity) ||
        !isFiniteNumber(body.questPercent) ||
        !isFiniteNumber(body.playerLevel) ||
        !isFiniteNumber(body.comboStreak) ||
        !isFiniteNumber(body.branchPressure) ||
        !isFiniteNumber(body.dependencyPulse) ||
        !isValidWorkflowDepth(body.workflowDepth) ||
        !isFiniteNumber(body.neuralRelevanceScore) ||
        !isValidNetworkDimensions(body.networkDimensions) ||
        !isFiniteNumber(body.modelConfidence) ||
        !isFiniteNumber(body.policyMomentum)) {
      return reply.status(400).send({error: 'Invalid netcode k3h4 payload'});
    }

    const k3h4 = await netcode.buildK3h4({
      callDensity: body.callDensity,
      questPercent: body.questPercent,
      playerLevel: body.playerLevel,
      comboStreak: body.comboStreak,
      branchPressure: body.branchPressure,
      dependencyPulse: body.dependencyPulse,
      workflowDepth: body.workflowDepth,
      neuralRelevanceScore: body.neuralRelevanceScore,
      networkDimensions: body.networkDimensions,
      modelConfidence: body.modelConfidence,
      policyMomentum: body.policyMomentum,
    });
    return {k3h4};
  });
}
