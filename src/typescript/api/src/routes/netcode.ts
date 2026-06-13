import type {FastifyInstance} from 'fastify';

import {getNativeNetcodeService, type NativeNetcodeService,} from '../services/nativeNetcode.ts';

type NetcodeRouteOptions = {
  netcodeService?: NativeNetcodeService;
};

export async function registerNetcodeRoutes(
    app: FastifyInstance,
    options: NetcodeRouteOptions = {},
    ): Promise<void> {
  const netcode = options.netcodeService ?? getNativeNetcodeService();

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
        if (!body || typeof body.callDensity !== 'number' ||
            typeof body.questPercent !== 'number' ||
            typeof body.comboStreak !== 'number' ||
            typeof body.branchPressure !== 'number' ||
            (body.workflowDepth !== 1 && body.workflowDepth !== 2 &&
             body.workflowDepth !== 3)) {
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
    if (!body || typeof body.callDensity !== 'number' ||
        typeof body.questPercent !== 'number' ||
        typeof body.comboStreak !== 'number' ||
        typeof body.branchPressure !== 'number' ||
        (body.workflowDepth !== 1 && body.workflowDepth !== 2 &&
         body.workflowDepth !== 3)) {
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
    if (!body || typeof body.callDensity !== 'number' ||
        typeof body.questPercent !== 'number' ||
        typeof body.playerLevel !== 'number' ||
        typeof body.comboStreak !== 'number' ||
        typeof body.branchPressure !== 'number' ||
        typeof body.dependencyPulse !== 'number') {
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
    const body = request.body;
    if (!body || typeof body.callDensity !== 'number' ||
        typeof body.questPercent !== 'number' ||
        typeof body.playerLevel !== 'number' ||
        typeof body.comboStreak !== 'number' ||
        typeof body.branchPressure !== 'number' ||
        typeof body.dependencyPulse !== 'number' ||
        (body.workflowDepth !== 1 && body.workflowDepth !== 2 &&
         body.workflowDepth !== 3) ||
        typeof body.networkDimensions !== 'number' ||
        typeof body.modelConfidence !== 'number' ||
        typeof body.policyMomentum !== 'number') {
      return reply.status(400).send(
          {error: 'Invalid netcode analytics payload'});
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
                                                     body.modelConfidence,
    );

    const link = await netcode.buildLink({
      callDensity: body.callDensity,
      questPercent: body.questPercent,
      playerLevel: body.playerLevel,
      comboStreak: body.comboStreak,
      branchPressure: body.branchPressure,
      dependencyPulse: body.dependencyPulse,
      interactionSignal: body.policyMomentum,
    });

    const vectorInput = {
      callDensity: body.callDensity,
      questPercent: body.questPercent,
      playerLevel: body.playerLevel,
      comboStreak: body.comboStreak,
      branchPressure: body.branchPressure,
      dependencyPulse: body.dependencyPulse,
      workflowDepth: body.workflowDepth,
      neuralRelevanceScore: reward.neuralRelevanceScore,
      networkDimensions: body.networkDimensions,
      modelConfidence: body.modelConfidence,
      policyMomentum: body.policyMomentum,
    };

    const vector = await netcode.buildVector(vectorInput);
    const hypersphere = await netcode.buildHypersphere(vectorInput);

    return {reward, link, vector, hypersphere};
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
    if (!body || typeof body.callDensity !== 'number' ||
        typeof body.questPercent !== 'number' ||
        typeof body.playerLevel !== 'number' ||
        typeof body.comboStreak !== 'number' ||
        typeof body.branchPressure !== 'number' ||
        typeof body.dependencyPulse !== 'number' ||
        (body.workflowDepth !== 1 && body.workflowDepth !== 2 &&
         body.workflowDepth !== 3) ||
        typeof body.neuralRelevanceScore !== 'number' ||
        typeof body.networkDimensions !== 'number' ||
        typeof body.modelConfidence !== 'number' ||
        typeof body.policyMomentum !== 'number') {
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
  }>('/api/netcode/hypersphere', async (request, reply) => {
    const body = request.body;
    if (!body || typeof body.callDensity !== 'number' ||
        typeof body.questPercent !== 'number' ||
        typeof body.playerLevel !== 'number' ||
        typeof body.comboStreak !== 'number' ||
        typeof body.branchPressure !== 'number' ||
        typeof body.dependencyPulse !== 'number' ||
        (body.workflowDepth !== 1 && body.workflowDepth !== 2 &&
         body.workflowDepth !== 3) ||
        typeof body.neuralRelevanceScore !== 'number' ||
        typeof body.networkDimensions !== 'number' ||
        typeof body.modelConfidence !== 'number' ||
        typeof body.policyMomentum !== 'number') {
      return reply.status(400).send(
          {error: 'Invalid netcode hypersphere payload'});
    }

    const hypersphere = await netcode.buildHypersphere({
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
    return {hypersphere};
  });
}
