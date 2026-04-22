type PrismaClientLike = {
  bananaApiCall?: {create(args: unknown): Promise<unknown>};
  bananaCalculation?: {create(args: unknown): Promise<unknown>};
  batchApiCall?: {create(args: unknown): Promise<unknown>};
  ripenessApiCall?: {create(args: unknown): Promise<unknown>};
  mlApiCall?: {create(args: unknown): Promise<unknown>};
};

let clientPromise: Promise<PrismaClientLike|null>|null = null;

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

export type DomainName = 'banana'|'batch'|'ripeness'|'ml';

export type BananaCalculationRecord = {
  purchases: number; multiplier: number; banana: number; message: string;
  source: 'native-c' | 'proxy';
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
      default:
        break;
    }
  } catch {
    // Keep proxy routes resilient while Prisma migrations are being rolled out.
  }
}
