type PrismaClientLike = {
  bananaApiCall?: {create(args: unknown): Promise<unknown>};
  bananaCalculation?: {create(args: unknown): Promise<unknown>};
  batchApiCall?: {create(args: unknown): Promise<unknown>};
  ripenessApiCall?: {create(args: unknown): Promise<unknown>};
  mlApiCall?: {create(args: unknown): Promise<unknown>};
  notBananaApiCall?: {create(args: unknown): Promise<unknown>};
  notBananaClassification?: {create(args: unknown): Promise<unknown>};
};

let clientPromise: Promise<PrismaClientLike|null>|null = null;

export function __setPrismaClientForTests(client: PrismaClientLike|null): void {
  clientPromise = Promise.resolve(client);
}

export function __resetPrismaClientForTests(): void {
  clientPromise = null;
}

export async function getPrismaClient(): Promise<PrismaClientLike|null> {
  if (clientPromise) {
    return clientPromise;
  }

  clientPromise = (async () => {
    try {
      const prismaModule = (await import('@prisma/client')) as
          {PrismaClient?: new () => PrismaClientLike};
      if (!prismaModule.PrismaClient) {
        return null;
      }

      return new prismaModule.PrismaClient();
    } catch {
      return null;
    }
  })();

  return clientPromise;
}

export type DomainName = 'banana'|'batch'|'ripeness'|'ml'|'not-banana';

export type BananaCalculationRecord = {
  purchases: number; multiplier: number; banana: number; message: string;
  source: 'native-c' | 'proxy';
};

export type NotBananaClassificationRecord = {
  classification: 'NOT_BANANA'|'MAYBE_BANANA'; bananaProbability: number;
  notBananaProbability: number;
  junkConfidence: number;
  actorCount: number;
  entityCount: number;
  requestBody: unknown;
  responseBody: unknown;
  source: string;
};

export async function saveBananaCalculation(record: BananaCalculationRecord):
    Promise<void> {
  const prisma = await getPrismaClient();
  if (!prisma?.bananaCalculation?.create) {
    return;
  }

  try {
    await prisma.bananaCalculation.create({
      data: {
        purchases: record.purchases,
        multiplier: record.multiplier,
        banana: record.banana,
        message: record.message,
        source: record.source,
      }
    });
  } catch {
    // Keep HTTP flow resilient when schema rollout lags runtime deployment.
  }
}

export async function saveNotBananaClassification(
    record: NotBananaClassificationRecord): Promise<void> {
  const prisma = await getPrismaClient();
  if (!prisma?.notBananaClassification?.create) {
    return;
  }

  try {
    await prisma.notBananaClassification.create({
      data: {
        classification: record.classification,
        bananaProbability: record.bananaProbability,
        notBananaProbability: record.notBananaProbability,
        junkConfidence: record.junkConfidence,
        actorCount: record.actorCount,
        entityCount: record.entityCount,
        requestJson: JSON.stringify(record.requestBody ?? null),
        responseJson: JSON.stringify(record.responseBody ?? null),
        source: record.source,
      }
    });
  } catch {
    // Keep HTTP flow resilient when schema rollout lags runtime deployment.
  }
}

export async function logDomainCall(
    domain: DomainName,
    route: string,
    statusCode: number,
    requestBody: unknown,
    responseBody: unknown,
    ): Promise<void> {
  const prisma = await getPrismaClient();
  if (!prisma) {
    return;
  }

  const data = {
    route,
    statusCode,
    requestJson: JSON.stringify(requestBody ?? null),
    responseJson: JSON.stringify(responseBody ?? null),
  };

  try {
    switch (domain) {
      case 'banana':
        await prisma.bananaApiCall?.create({data});
        break;
      case 'batch':
        await prisma.batchApiCall?.create({data});
        break;
      case 'ripeness':
        await prisma.ripenessApiCall?.create({data});
        break;
      case 'ml':
        await prisma.mlApiCall?.create({data});
        break;
      case 'not-banana':
        await prisma.notBananaApiCall?.create({data});
        break;
      default:
        break;
    }
  } catch {
    // Keep proxy routes resilient while Prisma migrations are being rolled out.
  }
}
