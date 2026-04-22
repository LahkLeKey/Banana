-- CreateTable
CREATE TABLE "BananaApiCall" (
    "id" BIGSERIAL NOT NULL,
    "route" TEXT NOT NULL,
    "statusCode" INTEGER NOT NULL,
    "requestJson" TEXT NOT NULL,
    "responseJson" TEXT NOT NULL,
    "createdAt" TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT "BananaApiCall_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "BananaCalculation" (
    "id" BIGSERIAL NOT NULL,
    "purchases" INTEGER NOT NULL,
    "multiplier" INTEGER NOT NULL,
    "banana" INTEGER NOT NULL,
    "message" TEXT NOT NULL,
    "source" TEXT NOT NULL,
    "createdAt" TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT "BananaCalculation_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "BatchApiCall" (
    "id" BIGSERIAL NOT NULL,
    "route" TEXT NOT NULL,
    "statusCode" INTEGER NOT NULL,
    "requestJson" TEXT NOT NULL,
    "responseJson" TEXT NOT NULL,
    "createdAt" TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT "BatchApiCall_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "RipenessApiCall" (
    "id" BIGSERIAL NOT NULL,
    "route" TEXT NOT NULL,
    "statusCode" INTEGER NOT NULL,
    "requestJson" TEXT NOT NULL,
    "responseJson" TEXT NOT NULL,
    "createdAt" TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT "RipenessApiCall_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "MlApiCall" (
    "id" BIGSERIAL NOT NULL,
    "route" TEXT NOT NULL,
    "statusCode" INTEGER NOT NULL,
    "requestJson" TEXT NOT NULL,
    "responseJson" TEXT NOT NULL,
    "createdAt" TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT "MlApiCall_pkey" PRIMARY KEY ("id")
);
