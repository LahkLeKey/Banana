-- CreateTable
CREATE TABLE "NotBananaApiCall" (
    "id" BIGSERIAL NOT NULL,
    "route" TEXT NOT NULL,
    "statusCode" INTEGER NOT NULL,
    "requestJson" TEXT NOT NULL,
    "responseJson" TEXT NOT NULL,
    "createdAt" TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT "NotBananaApiCall_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "NotBananaClassification" (
    "id" BIGSERIAL NOT NULL,
    "classification" TEXT NOT NULL,
    "bananaProbability" DOUBLE PRECISION NOT NULL,
    "notBananaProbability" DOUBLE PRECISION NOT NULL,
    "junkConfidence" DOUBLE PRECISION NOT NULL,
    "actorCount" INTEGER NOT NULL,
    "entityCount" INTEGER NOT NULL,
    "requestJson" TEXT NOT NULL,
    "responseJson" TEXT NOT NULL,
    "source" TEXT NOT NULL,
    "createdAt" TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT "NotBananaClassification_pkey" PRIMARY KEY ("id")
);
